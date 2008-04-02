/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: salbmp.cxx,v $
 *
 *  $Revision: 1.31 $
 *
 *  last change: $Author: kz $ $Date: 2008-04-02 09:50:46 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_vcl.hxx"

#include "tools/color.hxx"
#include "salbmp.h"
#include "vcl/bitmap.hxx" // for BitmapSystemData
#include "vcl/salbtype.hxx"

#include "basebmp/scanlineformats.hxx"
#include "basebmp/color.hxx"
#include "basegfx/vector/b2ivector.hxx"

#include <boost/bind.hpp>

#include "salinst.h"

// =======================================================================

static bool isValidBitCount( sal_uInt16 nBitCount )
{
    return (nBitCount == 1) || (nBitCount == 4) || (nBitCount == 8) || (nBitCount == 16) || (nBitCount == 24) || (nBitCount == 32);
}

// =======================================================================

AquaSalBitmap::AquaSalBitmap()
: mxGraphicContext( 0 )
, mnBits(0)
, mnWidth(0)
, mnHeight(0)
, mnBytesPerRow(0)
{
}

// ------------------------------------------------------------------

AquaSalBitmap::~AquaSalBitmap()
{
    Destroy();
}

// ------------------------------------------------------------------

bool AquaSalBitmap::Create( CGContextRef xContext, int nX, int nY, int nWidth, int nHeight, bool bMirrorVert )
{
    return Create(  CGBitmapContextGetWidth( xContext ), CGBitmapContextGetHeight( xContext ),
                    CGBitmapContextGetBitsPerPixel( xContext ), CGBitmapContextGetBytesPerRow( xContext ),
                    static_cast< sal_uInt8* >( CGBitmapContextGetData( xContext ) ),
                    nX, nY, nWidth, nHeight, bMirrorVert );
}

/** creates an AquaSalBitmap from a rectangle inside a memory bitmap (only 16 und 32 bit supported!)
    NOTE: This code flips the data upside down so sources must come from AQUA directly
*/
bool AquaSalBitmap::Create( int nSrcWidth, int nSrcHeight, int nBits,
    sal_uInt32 nBytesPerRow, const sal_uInt8* pSrcBuffer,
    int nSrcX, int nSrcY, int nDstWidth, int nDstHeight, bool bMirrorVert )
{
    if( (nBits != 16) && (nBits != 32) )
        return false;

    if( nSrcX < 0 )
        nSrcX = 0;
    if( nSrcY < 0 )
        nSrcY = 0;
    if( nDstWidth > nSrcWidth - nSrcX )
        nDstWidth = nSrcWidth - nSrcX;
    if( nDstHeight > nSrcHeight - nSrcY )
        nDstHeight = nSrcHeight - nSrcY;
    if( (nDstWidth <= 0) || (nDstHeight <= 0) )
        return false;

    mnBits = nBits;
    mnWidth  = nDstWidth;
    mnHeight = nDstHeight;

    if( AllocateUserData() )
    {
        sal_uInt8* pDest = maUserBuffer.get();
        const sal_uInt8* pSource = pSrcBuffer + nSrcX * (nBits / 8);
        const int nByteCopyWidth = nDstWidth * (nBits / 8);

        if( bMirrorVert )
        {
            nSrcY = (nSrcHeight - nSrcY) - nDstHeight;
            pSource += nBytesPerRow * nSrcY;

            int y = nDstHeight;
            while( y-- )
            {
                memcpy( pDest, pSource, nByteCopyWidth );
                pDest += mnBytesPerRow;
                pSource += nBytesPerRow;
            }
        }
        else
        {
            pSource += nBytesPerRow * nSrcY;
            pDest += mnBytesPerRow * (nDstHeight-1);

            int y = nDstHeight;
            while( y-- )
            {
                memcpy( pDest, pSource, nByteCopyWidth );
                pDest -= mnBytesPerRow;
                pSource += nBytesPerRow;
            }
        }
        return true;
    }
    return false;
}

// ------------------------------------------------------------------

bool AquaSalBitmap::Create( CGImageRef& xImage )
{
    mnBits = CGImageGetBitsPerPixel( xImage );
    if( (mnBits == 16) || (mnBits == 32) )
    {
        mnWidth = CGImageGetWidth( xImage );
        mnHeight = CGImageGetHeight( xImage );

        if( AllocateUserData() )
        {
            CreateContext();
            if( mxGraphicContext )
            {
                CGContextDrawImage(mxGraphicContext, CGRectMake( 0, 0, mnWidth, mnHeight ), xImage);
                return true;
            }
        }
    }
    return false;
}

// ------------------------------------------------------------------

bool AquaSalBitmap::Create( const Size& rSize, USHORT nBits, const BitmapPalette& rBitmapPalette )
{
    if( !isValidBitCount( nBits ) )
        return false;
    maPalette = rBitmapPalette;
    mnBits = nBits;
    mnWidth = rSize.Width();
    mnHeight = rSize.Height();
    return AllocateUserData();
}

// ------------------------------------------------------------------

bool AquaSalBitmap::Create( const SalBitmap& rSalBmp )
{
    return Create( rSalBmp, rSalBmp.GetBitCount() );
}

// ------------------------------------------------------------------

bool AquaSalBitmap::Create( const SalBitmap& rSalBmp, SalGraphics* pGraphics )
{
    return Create( rSalBmp, pGraphics ? pGraphics->GetBitCount() : rSalBmp.GetBitCount() );
}

// ------------------------------------------------------------------

bool AquaSalBitmap::Create( const SalBitmap& rSalBmp, USHORT nNewBitCount )
{
    const AquaSalBitmap& rSourceBitmap = static_cast<const AquaSalBitmap&>(rSalBmp);

    if( isValidBitCount( nNewBitCount ) &&  rSourceBitmap.maUserBuffer.get() )
    {
        mnBits = nNewBitCount;
        mnWidth = rSourceBitmap.mnWidth;
        mnHeight = rSourceBitmap.mnHeight;
        maPalette = rSourceBitmap.maPalette;

        if( AllocateUserData() )
        {
            ConvertBitmapData( mnWidth, mnHeight, mnBits, mnBytesPerRow, maPalette, maUserBuffer.get(), rSourceBitmap.mnBits, rSourceBitmap.mnBytesPerRow, rSourceBitmap.maPalette, rSourceBitmap.maUserBuffer.get() );
            return true;
        }
    }
    return false;
}

// ------------------------------------------------------------------

void AquaSalBitmap::Destroy()
{
    DestroyContext();
    maUserBuffer.reset();
}

// ------------------------------------------------------------------

void AquaSalBitmap::DestroyContext()
{
    if( mxGraphicContext )
    {
        CFRelease( mxGraphicContext );
        mxGraphicContext = 0;
        maContextBuffer.reset();
    }
}

// ------------------------------------------------------------------

bool AquaSalBitmap::CreateContext()
{
    DestroyContext();

    size_t bitsPerComponent = (mnBits == 16) ? 5 : 8;
    sal_uInt32 nContextBytesPerRow = mnBytesPerRow;
    if( (mnBits == 16) || (mnBits ==32) )
    {
        // simple, no conversion needed
        maContextBuffer = maUserBuffer;
    }
    else
    {
        // convert user data to 32 bit
        nContextBytesPerRow = mnWidth << 2;
        maContextBuffer.reset( new sal_uInt8[ mnHeight * nContextBytesPerRow ] );

        ConvertBitmapData( mnWidth, mnHeight,
                           32, nContextBytesPerRow, maPalette, maContextBuffer.get(),
                           mnBits, mnBytesPerRow, maPalette, maUserBuffer.get() );
    }

    mxGraphicContext = CGBitmapContextCreate( maContextBuffer.get(), mnWidth, mnHeight, bitsPerComponent, nContextBytesPerRow, GetSalData()->mxRGBSpace, kCGImageAlphaNoneSkipFirst );

    if( mxGraphicContext )
    {
        CGContextTranslateCTM (mxGraphicContext, 0, mnHeight);
        CGContextScaleCTM (mxGraphicContext, 1.0, -1.0);
    }
    else
    {
        maContextBuffer.reset();
    }

    return mxGraphicContext != NULL;
}

// ------------------------------------------------------------------

bool AquaSalBitmap::AllocateUserData()
{
    Destroy();

    if( mnWidth && mnHeight )
    {
        mnBytesPerRow =  0;

        switch( mnBits )
        {
        case 1:     mnBytesPerRow = (mnWidth + 7) >> 3; break;
        case 4:     mnBytesPerRow = (mnWidth + 1) >> 1; break;
        case 8:     mnBytesPerRow = mnWidth; break;
        case 16:    mnBytesPerRow = mnWidth << 1; break;
        case 24:    mnBytesPerRow = (mnWidth << 1) + mnWidth; break;
        case 32:    mnBytesPerRow = mnWidth << 2; break;
        default:
            DBG_ERROR("vcl::AquaSalBitmap::AllocateUserData(), illegal bitcount!");
        }
    }

    if( mnBytesPerRow )
        maUserBuffer.reset( new sal_uInt8[mnBytesPerRow * mnHeight] );

    return maUserBuffer.get() != 0;
}

// ------------------------------------------------------------------

class ImplPixelFormat
{
protected:
    sal_uInt8* pData;
public:
    static ImplPixelFormat* GetFormat( sal_uInt16 nBits, const BitmapPalette& rPalette );

    virtual void StartLine( sal_uInt8* pLine ) { pData = pLine; }
    virtual void SkipPixel( sal_uInt32 nPixel ) = 0;
    virtual ColorData ReadPixel() = 0;
    virtual void WritePixel( ColorData nColor ) = 0;
};

class ImplPixelFormat32 : public ImplPixelFormat
{
public:
    virtual void SkipPixel( sal_uInt32 nPixel )
    {
        pData += nPixel << 2;
    }
    virtual ColorData ReadPixel()
    {
        pData++;
        return RGB_COLORDATA( *pData++, *pData++, *pData++ );
    }
    virtual void WritePixel( ColorData nColor )
    {
        *pData++ = 0;
        *pData++ = COLORDATA_RED( nColor );
        *pData++ = COLORDATA_GREEN( nColor );
        *pData++ = COLORDATA_BLUE( nColor );
    }
};

class ImplPixelFormat24 : public ImplPixelFormat
{
public:
    virtual void SkipPixel( sal_uInt32 nPixel )
    {
        pData += (nPixel << 1) + nPixel;
    }
    virtual ColorData ReadPixel()
    {
        const ColorData c = RGB_COLORDATA( pData[2], pData[1], pData[0] );
        pData += 3;
        return c;
    }
    virtual void WritePixel( ColorData nColor )
    {
        *pData++ = COLORDATA_BLUE( nColor );
        *pData++ = COLORDATA_GREEN( nColor );
        *pData++ = COLORDATA_RED( nColor );
    }
};

class ImplPixelFormat16 : public ImplPixelFormat
{
protected:
    sal_uInt16* pData16;
public:

    virtual void StartLine( sal_uInt8* pLine )
    {
        pData16 = (sal_uInt16*)pLine;
    }
    virtual void SkipPixel( sal_uInt32 nPixel )
    {
        pData += nPixel;
    }
    virtual ColorData ReadPixel()
    {
        const ColorData c = RGB_COLORDATA( (*pData & 0x7c00) >> 7, (*pData & 0x03e0) >> 2 , (*pData & 0x001f) << 3 );
        pData++;
        return c;
    }
    virtual void WritePixel( ColorData nColor )
    {
        *pData++ =  ((COLORDATA_RED( nColor ) & 0xf8 ) << 7 ) ||
                    ((COLORDATA_GREEN( nColor ) & 0xf8 ) << 2 ) ||
                    ((COLORDATA_BLUE( nColor ) & 0xf8 ) >> 3 );
    }
};

class ImplPixelFormat8 : public ImplPixelFormat
{
private:
    const BitmapPalette& mrPalette;

public:
    ImplPixelFormat8( const BitmapPalette& rPalette )
    : mrPalette( rPalette )
    {
    }
    virtual void SkipPixel( sal_uInt32 nPixel )
    {
        pData += nPixel;
    }
    virtual ColorData ReadPixel()
    {
        return mrPalette[ *pData++ ].operator Color().GetColor();
    }
    virtual void WritePixel( ColorData nColor )
    {
        const BitmapColor aColor( COLORDATA_RED( nColor ), COLORDATA_GREEN( nColor ), COLORDATA_BLUE( nColor ) );
        *pData++ = static_cast< sal_uInt8 >( mrPalette.GetBestIndex( aColor ) );
    }
};

class ImplPixelFormat4 : public ImplPixelFormat
{
private:
    const BitmapPalette& mrPalette;
    sal_uInt32 mnX;
    sal_uInt32 mnShift;

public:
    ImplPixelFormat4( const BitmapPalette& rPalette )
    : mrPalette( rPalette )
    {
    }
    virtual void SkipPixel( sal_uInt32 nPixel )
    {
        mnX += nPixel;
        if( (nPixel & 1) )
            mnShift ^= 4;
    }
    virtual void StartLine( sal_uInt8* pLine )
    {
        pData = pLine;
        mnX = 0;
        mnShift = 4;
    }
    virtual ColorData ReadPixel()
    {
        const BitmapColor& rColor = mrPalette[( pData[mnX >> 1] >> mnShift) & 0x0f];
        mnX++;
        mnShift ^= 4;
        return rColor.operator Color().GetColor();
    }
    virtual void WritePixel( ColorData nColor )
    {
        const BitmapColor aColor( COLORDATA_RED( nColor ), COLORDATA_GREEN( nColor ), COLORDATA_BLUE( nColor ) );
        pData[mnX>>1] &= (0xf0 >> mnShift);
        pData[mnX>>1] |= (static_cast< sal_uInt8 >( mrPalette.GetBestIndex( aColor ) ) & 0x0f);
        mnX++;
        mnShift ^= 4;
    }
};

class ImplPixelFormat1 : public ImplPixelFormat
{
private:
    const BitmapPalette& mrPalette;
    sal_uInt32 mnX;

public:
    ImplPixelFormat1( const BitmapPalette& rPalette )
    : mrPalette( rPalette )
    {
    }
    virtual void SkipPixel( sal_uInt32 nPixel )
    {
        mnX += nPixel;
    }
    virtual void StartLine( sal_uInt8* pLine )
    {
        pData = pLine;
        mnX = 0;
    }
    virtual ColorData ReadPixel()
    {
        const BitmapColor& rColor = mrPalette[ (pData[mnX >> 3 ] >> ( 7 - ( mnX & 7 ) )) & 1];
        mnX++;
        return rColor.operator Color().GetColor();
    }
    virtual void WritePixel( ColorData nColor )
    {
        const BitmapColor aColor( COLORDATA_RED( nColor ), COLORDATA_GREEN( nColor ), COLORDATA_BLUE( nColor ) );
        if( mrPalette.GetBestIndex( aColor ) & 1 )
            pData[ mnX >> 3 ] |= 1 << ( 7 - ( mnX & 7 ) );
        else
            pData[ mnX >> 3 ] &= ~( 1 << ( 7 - ( mnX & 7 ) ) );
        mnX++;
    }
};

ImplPixelFormat* ImplPixelFormat::GetFormat( sal_uInt16 nBits, const BitmapPalette& rPalette )
{
    switch( nBits )
    {
    case 1: return new ImplPixelFormat1( rPalette );
    case 4: return new ImplPixelFormat4( rPalette );
    case 8: return new ImplPixelFormat8( rPalette );
    case 16: return new ImplPixelFormat16;
    case 24: return new ImplPixelFormat24;
    case 32: return new ImplPixelFormat32;
    }

    return 0;
}

basebmp::BitmapDeviceSharedPtr AquaSalBitmap::getBitmapDevice() const
{
    sal_Int32 nFormat = basebmp::Format::NONE;
    basebmp::PaletteMemorySharedVector aPalette;
    switch( mnBits )
    {
    case 1:  nFormat = basebmp::Format::ONE_BIT_MSB_PAL;            break;
    case 4:  nFormat = basebmp::Format::FOUR_BIT_MSB_PAL;           break;
    case 8:  nFormat = basebmp::Format::EIGHT_BIT_PAL;              break;
    case 16: nFormat = basebmp::Format::SIXTEEN_BIT_MSB_TC_MASK;    break;
    case 24: nFormat = basebmp::Format::TWENTYFOUR_BIT_TC_MASK;     break;
    case 32: nFormat = basebmp::Format::THIRTYTWO_BIT_TC_MASK_ARGB; break;
    default:
        DBG_ERROR( "unsupported bitmap format" );
        break;
    }

    if( mnBits < 16 )
    {
        USHORT nColors = maPalette.GetEntryCount();
        std::vector<basebmp::Color>* pVector = new std::vector<basebmp::Color>( nColors );
        for( USHORT nCol = 0; nCol < nColors; nCol++ )
        {
            const BitmapColor& rCol( maPalette[nCol] );
            (*pVector)[nCol] = basebmp::Color( rCol.GetRed(), rCol.GetGreen(), rCol.GetBlue() );
        }
        aPalette.reset( pVector );
    }

    return basebmp::createBitmapDevice( basegfx::B2IVector( mnWidth, mnHeight ),
                                        false,
                                        nFormat,
                                        maUserBuffer,
                                        aPalette );
}

void AquaSalBitmap::ConvertBitmapData( sal_uInt32 nWidth, sal_uInt32 nHeight,
                                       sal_uInt16 nDestBits, sal_uInt32 nDestBytesPerRow, const BitmapPalette& rDestPalette, sal_uInt8* pDestData,
                                       sal_uInt16 nSrcBits, sal_uInt32 nSrcBytesPerRow, const BitmapPalette& rSrcPalette, sal_uInt8* pSrcData )

{
    if( (nDestBytesPerRow == nSrcBytesPerRow) && (nDestBits == nSrcBits) && ((nSrcBits != 8) || (rDestPalette.operator==( rSrcPalette ))) )
    {
        // simple case, same format, so just copy
        memcpy( pDestData, pSrcData, nHeight * nDestBytesPerRow );
    }
    else
    {
        // TODO: this implementation is for clarety, not for speed

        ImplPixelFormat* pD = ImplPixelFormat::GetFormat( nDestBits, rDestPalette );
        ImplPixelFormat* pS = ImplPixelFormat::GetFormat( nSrcBits, rSrcPalette );

        if( pD && pS )
        {
            sal_uInt32 nY = nHeight;
            while( nY-- )
            {
                pD->StartLine( pDestData );
                pS->StartLine( pSrcData );

                sal_uInt32 nX = nWidth;
                while( nX-- )
                    pD->WritePixel( pS->ReadPixel() );

                pSrcData += nSrcBytesPerRow;
                pDestData += nDestBytesPerRow;
            }
        }
        delete pS;
        delete pD;
    }
}

// ------------------------------------------------------------------

Size AquaSalBitmap::GetSize() const
{
    return Size( mnWidth, mnHeight );
}

// ------------------------------------------------------------------

USHORT AquaSalBitmap::GetBitCount() const
{
    return mnBits;
}

// ------------------------------------------------------------------

BitmapBuffer* AquaSalBitmap::AcquireBuffer( bool bReadOnly )
{
    BitmapBuffer* pBuffer = new BitmapBuffer;
    pBuffer->mnWidth = mnWidth;
    pBuffer->mnHeight = mnHeight;
    pBuffer->maPalette = maPalette;
    pBuffer->mnScanlineSize = mnBytesPerRow;
    pBuffer->mpBits = maUserBuffer.get();
    pBuffer->mnBitCount = mnBits;
    switch( mnBits )
    {
    case 1:     pBuffer->mnFormat = BMP_FORMAT_1BIT_MSB_PAL; break;
    case 4:     pBuffer->mnFormat = BMP_FORMAT_4BIT_MSN_PAL; break;
    case 8:     pBuffer->mnFormat = BMP_FORMAT_8BIT_PAL; break;
    case 16:    pBuffer->mnFormat = BMP_FORMAT_16BIT_TC_MSB_MASK;
                pBuffer->maColorMask  = ColorMask( k16BitRedColorMask, k16BitGreenColorMask, k16BitBlueColorMask );
                break;
    case 24:    pBuffer->mnFormat = BMP_FORMAT_24BIT_TC_BGR; break;
    case 32:    pBuffer->mnFormat = BMP_FORMAT_32BIT_TC_ARGB;
                pBuffer->maColorMask  = ColorMask( k32BitRedColorMask, k32BitGreenColorMask, k32BitBlueColorMask );
                break;
    }

    return pBuffer;
}

// ------------------------------------------------------------------

void AquaSalBitmap::ReleaseBuffer( BitmapBuffer* pBuffer, bool bReadOnly )
{
    // invalidate graphic context if we have different data
    if( !bReadOnly )
    {
        maPalette = pBuffer->maPalette;
        if( mxGraphicContext )
        DestroyContext();
    }

    delete pBuffer;
}

// ------------------------------------------------------------------

CGImageRef AquaSalBitmap::CreateCroppedImage( int nX, int nY, int nWidth, int nHeight )
{
    CGImageRef xImage = 0;

    if( !mxGraphicContext )
        CreateContext();

    if( mxGraphicContext )
        xImage = CreateCroppedImage( mxGraphicContext, nX, nY, nWidth, nHeight );

    return xImage;
}

// ------------------------------------------------------------------

CGImageRef AquaSalBitmap::CreateCroppedImage( CGContextRef xContext, int nX, int nY, int nWidth, int nHeight )
{
    CGImageRef xImage = 0;
    if( xContext )
    {
        xImage = CGBitmapContextCreateImage( xContext );
        if( nX || nY || (CGImageGetWidth(xImage) != static_cast<unsigned int> (nWidth)) || (CGImageGetHeight(xImage) != static_cast<unsigned int>(nHeight)) )
        {
            CGImageRef myTempImage = CGImageCreateWithImageInRect( xImage, CGRectMake( nX, nY, nWidth, nHeight ) );
            CGImageRelease( xImage );
            xImage = myTempImage;
        }
    }
    return xImage;
}

// ------------------------------------------------------------------

static void CFRTLFree(void* /*info*/, const void* data, size_t /*size*/)
{
    rtl_freeMemory( const_cast<void*>(data) );
}

/** TODO: Optimize me, merge the bitmap and alpha mask ourself
*/
CGImageRef AquaSalBitmap::CreateWithMask( const AquaSalBitmap& rMask, sal_uInt32 nX, sal_uInt32 nY, sal_uInt32 nDX, sal_uInt32 nDY )
{
    AquaSalBitmap rMaskNew;
    if(!rMaskNew.Create( rMask ))
        return NULL;


    CGImageRef xMaskedImage = 0;

    CGImageRef xImage( CreateCroppedImage( nX, nY, nDX, nDY ) );
    if( xImage )
    {
        CGImageRef xMask = rMaskNew.CreateCroppedImage( nX, nY, nDX, nDY );
        if( xMask )
        {

            rMaskNew.Destroy();//destroy old context with a size which could be different from
            //the xMaskedImage, xImage, xMask size (the three have the same size=(nDX, nDY))
            // and are part of the original image

            CGRect xImageRect=CGRectMake( 0, 0, nDX, nDY);//the rect has no offset

            // create the alpha mask image fitting our image
            int nMaskBytesPerRow = ((nDX + 3 ) & ~3);
            void* pMaskMem = rtl_allocateMemory( nMaskBytesPerRow * nDY );
            CGContextRef xMaskContext = CGBitmapContextCreate( pMaskMem, nDX, nDY, 8, nMaskBytesPerRow, GetSalData()->mxGraySpace, kCGImageAlphaNone );
            CGContextDrawImage( xMaskContext, xImageRect, xMask );
            CFRelease( xMask );
            CGDataProviderRef xDataProvider( CGDataProviderCreateWithData(NULL, pMaskMem, nDY * nMaskBytesPerRow, &CFRTLFree) );
            xMask = CGImageMaskCreate( nDX, nDY, 8, 8, nMaskBytesPerRow, xDataProvider, NULL, false );
            CFRelease( xDataProvider );
            CFRelease( xMaskContext );

            // combine image and alpha mask
            xMaskedImage = CGImageCreateWithMask( xImage, xMask );

            CFRelease( xMask );
        }
        CFRelease( xImage );
    }
    return xMaskedImage;
}

// ------------------------------------------------------------------

/** creates an image from the given rectangle, replacing all black pixels with nMaskColor and make all other full transparent */
CGImageRef AquaSalBitmap::CreateColorMask( int nX, int nY, int nWidth, int nHeight, SalColor nMaskColor ) const
{
    CGImageRef xMask = 0;
    if( maUserBuffer.get() && (nX + nWidth <= mnWidth) && (nY + nHeight <= mnHeight) )
    {
        const sal_uInt32 nDestBytesPerRow = nWidth << 2;
        sal_uInt32* pMaskBuffer = static_cast<sal_uInt32*>( rtl_allocateMemory( nHeight * nDestBytesPerRow ) );
        sal_uInt32* pDest = pMaskBuffer;

        ImplPixelFormat* pSourcePixels = ImplPixelFormat::GetFormat( mnBits, maPalette );

        if( pMaskBuffer && pSourcePixels )
        {
            sal_uInt32 nColor;
            reinterpret_cast<sal_uInt8*>(&nColor)[0] = 0xff;
            reinterpret_cast<sal_uInt8*>(&nColor)[1] = SALCOLOR_RED( nMaskColor );
            reinterpret_cast<sal_uInt8*>(&nColor)[2] = SALCOLOR_GREEN( nMaskColor );
            reinterpret_cast<sal_uInt8*>(&nColor)[3] = SALCOLOR_BLUE( nMaskColor );

            sal_uInt8* pSource = maUserBuffer.get();
            if( nY )
                pSource += nY * mnBytesPerRow;

            int y = nHeight;
            while( y-- )
            {
                pSourcePixels->StartLine( pSource );
                pSourcePixels->SkipPixel(nX);
                sal_uInt32 x = nWidth;
                while( x-- )
                {
                    *pDest++ = ( pSourcePixels->ReadPixel() == 0 ) ? nColor : 0;
                }
                pSource += mnBytesPerRow;
            }

            CGDataProviderRef xDataProvider( CGDataProviderCreateWithData(NULL, pMaskBuffer, nHeight * nDestBytesPerRow, &CFRTLFree) );
            xMask = CGImageCreate(nWidth, nHeight, 8, 32, nDestBytesPerRow, GetSalData()->mxRGBSpace, kCGImageAlphaPremultipliedFirst, xDataProvider, NULL, true, kCGRenderingIntentDefault);
            CFRelease(xDataProvider);
        }
        else
        {
            free(pMaskBuffer);
        }

        delete pSourcePixels;
    }
    return xMask;
}

// ------------------------------------------------------------------

CGImageRef AquaSalBitmap::CreateMask( int nX, int nY, int nWidth, int nHeight ) const
{
    CGImageRef xMask = 0;
    if( (maUserBuffer.get()) && (nX + nWidth <= mnWidth) && (nY + nHeight <= mnHeight) )
    {
        if( mnBits == 1 )
        {
            CGDataProviderRef xDataProvider( CGDataProviderCreateWithData(NULL, maUserBuffer.get(), nHeight * mnBytesPerRow, NULL) );
            xMask = CGImageMaskCreate(nWidth, nHeight, 1, 1, mnBytesPerRow, xDataProvider, NULL, true );
            CFRelease(xDataProvider);
        }
        else
        {
            basebmp::RawMemorySharedArray aMaskBuffer( new sal_uInt8[ nWidth * nHeight ] );
            sal_uInt8* pDest = aMaskBuffer.get();
            if( pDest )
            {
                sal_uInt8* pSource = maUserBuffer.get();
                if( nY )
                    pSource += nY * mnBytesPerRow;

                if( mnBits == 8 )
                {
                    if( nX )
                        pSource += nX;

                    // simple convert alpha
                    int y = nHeight;
                    while( y-- )
                    {
                        int x = nWidth;
                        while( x-- )
                            *pDest++ = 0xff - *pSource++;
                        pSource += mnBytesPerRow;
                    }
                }
                else
                {
                    ImplPixelFormat* pSourcePixels = ImplPixelFormat::GetFormat( mnBits, maPalette );
                    if( pSourcePixels )
                    {
                        int y = nHeight;
                        while( y-- )
                        {
                            pSourcePixels->StartLine(pSource);
                            pSourcePixels->SkipPixel(nX);
                            int x = nWidth;
                            while( x-- )
                                *pDest++ = (pSourcePixels->ReadPixel() == 0) ? 0x00 : 0xff;

                            pSource += mnBytesPerRow;
                        }
                        delete pSourcePixels;
                    }
                }

                CGDataProviderRef xDataProvider( CGDataProviderCreateWithData(NULL, aMaskBuffer.get(), nHeight * nWidth, NULL) );
                xMask = CGImageMaskCreate(nWidth, nHeight, 8, 8, nWidth, xDataProvider, NULL, true );
                CFRelease(xDataProvider);
            }
        }
    }
    return xMask;
}

// =======================================================================

/** AquaSalBitmap::GetSystemData Get platform native image data from existing image
 *
 *  @param rData struct BitmapSystemData, defined in vcl/inc/bitmap.hxx
 *  @return true if successful
**/
bool AquaSalBitmap::GetSystemData( BitmapSystemData& rData )
{
    bool bRet = false;

    if( !mxGraphicContext )
        CreateContext();

    if ( mxGraphicContext )
    {
        bRet = true;

#ifdef CAIRO
        if ((CGBitmapContextGetBitmapInfo(mxGraphicContext) & kCGBitmapByteOrderMask) != kCGBitmapByteOrder32Host) {
            /**
             * We need to hack things because VCL does not use kCGBitmapByteOrder32Host, while Cairo requires it.
             */
            OSL_TRACE("AquaSalBitmap::%s(): kCGBitmapByteOrder32Host not found => inserting it.",__func__);

            CGImageRef xImage = CGBitmapContextCreateImage (mxGraphicContext);

            // re-create the context with single change: include kCGBitmapByteOrder32Host flag.
            CGContextRef mxGraphicContextNew = CGBitmapContextCreate( CGBitmapContextGetData(mxGraphicContext),
                                                                      CGBitmapContextGetWidth(mxGraphicContext),
                                                                      CGBitmapContextGetHeight(mxGraphicContext),
                                                                      CGBitmapContextGetBitsPerComponent(mxGraphicContext),
                                                                      CGBitmapContextGetBytesPerRow(mxGraphicContext),
                                                                      CGBitmapContextGetColorSpace(mxGraphicContext),
                                                                      CGBitmapContextGetBitmapInfo(mxGraphicContext) | kCGBitmapByteOrder32Host);
            CFRelease(mxGraphicContext);

            // Needs to be flipped
            CGContextSaveGState( mxGraphicContextNew );
            CGContextTranslateCTM (mxGraphicContextNew, 0, CGBitmapContextGetHeight(mxGraphicContextNew));
            CGContextScaleCTM (mxGraphicContextNew, 1.0, -1.0);

            CGContextDrawImage(mxGraphicContextNew, CGRectMake( 0, 0, CGImageGetWidth(xImage), CGImageGetHeight(xImage)), xImage);

            // Flip back
            CGContextRestoreGState( mxGraphicContextNew );

            CGImageRelease( xImage );
            mxGraphicContext = mxGraphicContextNew;
        }
#endif

        rData.rImageContext = (void *) mxGraphicContext;
        rData.mnWidth = (int) CGBitmapContextGetWidth(mxGraphicContext);
        rData.mnHeight = (int) CGBitmapContextGetHeight(mxGraphicContext);
    }

    return bRet;
}
