/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svtools.hxx"

#include <string.h>
#include <stdio.h>
#include <tools/stream.hxx>
#include <tools/debug.hxx>
#include <vcl/outdev.hxx>
#include <tools/config.hxx>
#include <svtools/filter.hxx>
#include "FilterConfigCache.hxx"
#include <unotools/ucbstreamhelper.hxx>

#define DATA_SIZE           640

BYTE* ImplSearchEntry( BYTE* , BYTE* , ULONG , ULONG  );

/*************************************************************************
|*
|*
|*
\************************************************************************/

GraphicDescriptor::GraphicDescriptor( const INetURLObject& rPath ) :
    pFileStm( ::utl::UcbStreamHelper::CreateStream( rPath.GetMainURL( INetURLObject::NO_DECODE ), STREAM_READ ) ),
    aPathExt( rPath.GetFileExtension().toAsciiLowerCase() ),
    bOwnStream( TRUE )
{
    ImpConstruct();
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

GraphicDescriptor::GraphicDescriptor( SvStream& rInStream, const String* pPath) :
    pFileStm    ( &rInStream ),
    bOwnStream  ( FALSE )
{
    ImpConstruct();

    if ( pPath )
    {
        INetURLObject aURL( *pPath );
        aPathExt = aURL.GetFileExtension().toAsciiLowerCase();
    }
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

GraphicDescriptor::~GraphicDescriptor()
{
    if ( bOwnStream )
        delete pFileStm;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::Detect( BOOL bExtendedInfo )
{
    BOOL bRet = FALSE;
    if ( pFileStm && !pFileStm->GetError() )
    {
        SvStream&   rStm = *pFileStm;
        UINT16      nOldFormat = rStm.GetNumberFormatInt();

        if      ( ImpDetectGIF( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectJPG( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectBMP( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectPNG( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectTIF( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectPCX( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectDXF( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectMET( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectSGF( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectSGV( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectSVM( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectWMF( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectEMF( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectPCT( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectXBM( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectXPM( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectPBM( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectPGM( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectPPM( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectRAS( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectTGA( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectPSD( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectEPS( rStm, bExtendedInfo ) ) bRet = TRUE;
        else if ( ImpDetectPCD( rStm, bExtendedInfo ) ) bRet = TRUE;

        rStm.SetNumberFormatInt( nOldFormat );
    }
    return bRet;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void GraphicDescriptor::ImpConstruct()
{
    nFormat = GFF_NOT;
    nBitsPerPixel = 0;
    nPlanes = 0;
    bCompressed = FALSE;
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectBMP( SvStream& rStm, BOOL bExtendedInfo )
{
    UINT16  nTemp16;
    BOOL    bRet = FALSE;
    sal_Int32 nStmPos = rStm.Tell();

    rStm.SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
    rStm >> nTemp16;

    // OS/2-BitmapArray
    if ( nTemp16 == 0x4142 )
    {
        rStm.SeekRel( 0x0c );
        rStm >> nTemp16;
    }

    // Bitmap
    if ( nTemp16 == 0x4d42 )
    {
        nFormat = GFF_BMP;
        bRet = TRUE;

        if ( bExtendedInfo )
        {
            UINT32  nTemp32;
            UINT32  nCompression;

            // bis zur ersten Information
            rStm.SeekRel( 0x10 );

            // PixelBreite auslesen
            rStm >> nTemp32;
            aPixSize.Width() = nTemp32;

            // PixelHoehe auslesen
            rStm >> nTemp32;
            aPixSize.Height() = nTemp32;

            // Planes auslesen
            rStm >> nTemp16;
            nPlanes = nTemp16;

            // BitCount auslesen
            rStm >> nTemp16;
            nBitsPerPixel = nTemp16;

            // Compression auslesen
            rStm >> nTemp32;
            bCompressed = ( ( nCompression = nTemp32 ) > 0 );

            // logische Breite
            rStm.SeekRel( 4 );
            rStm >> nTemp32;
            if ( nTemp32 )
                aLogSize.Width() = ( aPixSize.Width() * 100000 ) / nTemp32;

            // logische Hoehe
            rStm >> nTemp32;
            if ( nTemp32 )
                aLogSize.Height() = ( aPixSize.Height() * 100000 ) / nTemp32;

            // Wir wollen noch etwas feiner differenzieren und
            // auf sinnvolle Werte ueberpruefen ( Bug-Id #29001 )
            if ( ( nBitsPerPixel > 24 ) || ( nCompression > 3 ) )
            {
                nFormat = GFF_NOT;
                bRet = FALSE;
            }
        }
    }
    rStm.Seek( nStmPos );
    return bRet;
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectGIF( SvStream& rStm, BOOL bExtendedInfo )
{
    UINT32  n32;
    UINT16  n16;
    BOOL    bRet = FALSE;
    BYTE    cByte;

    sal_Int32 nStmPos = rStm.Tell();
    rStm.SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
    rStm >> n32;

    if ( n32 == 0x38464947 )
    {
        rStm >> n16;
        if ( ( n16 == 0x6137 ) || ( n16 == 0x6139 ) )
        {
            nFormat = GFF_GIF;
            bRet = TRUE;

            if ( bExtendedInfo )
            {
                UINT16 nTemp16;

                // PixelBreite auslesen
                rStm >> nTemp16;
                aPixSize.Width() = nTemp16;

                // PixelHoehe auslesen
                rStm >> nTemp16;
                aPixSize.Height() = nTemp16;

                // Bits/Pixel auslesen
                rStm >> cByte;
                nBitsPerPixel = ( ( cByte & 112 ) >> 4 ) + 1;
            }
        }
    }
    rStm.Seek( nStmPos );
    return bRet;
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

// returns the next jpeg marker, a return value of 0 represents an error
sal_uInt8 ImpDetectJPG_GetNextMarker( SvStream& rStm )
{
    sal_uInt8 nByte;
    do
    {
        do
        {
            rStm >> nByte;
            if ( rStm.IsEof() || rStm.GetError() )  // as 0 is not allowed as marker,
                return 0;                           // we can use it as errorcode
        }
        while ( nByte != 0xff );
        do
        {
            rStm >> nByte;
            if ( rStm.IsEof() || rStm.GetError() )
                return 0;
        }
        while( nByte == 0xff );
    }
    while( nByte == 0 );        // 0xff00 represents 0xff and not a marker,
                                // the marker detection has to be restartet.
    return nByte;
}

BOOL GraphicDescriptor::ImpDetectJPG( SvStream& rStm,  BOOL bExtendedInfo )
{
    UINT32  nTemp32;
    BOOL    bRet = FALSE;

    sal_Int32 nStmPos = rStm.Tell();

    rStm.SetNumberFormatInt( NUMBERFORMAT_INT_BIGENDIAN );
    rStm >> nTemp32;

    // compare upper 24 bits
    if( 0xffd8ff00 == ( nTemp32 & 0xffffff00 ) )
    {
        nFormat = GFF_JPG;
        bRet = TRUE;

        if ( bExtendedInfo )
        {
            rStm.SeekRel( -2 );

            sal_uInt32 nError( rStm.GetError() );

            sal_Bool bScanFailure = sal_False;
            sal_Bool bScanFinished = sal_False;

            while( !bScanFailure && !bScanFinished && !rStm.IsEof() && !rStm.GetError() )
            {
                sal_uInt8 nMarker = ImpDetectJPG_GetNextMarker( rStm );
                switch( nMarker )
                {
                    // fixed size marker, not having a two byte length parameter
                    case 0xd0 :     // RST0
                    case 0xd1 :
                    case 0xd2 :
                    case 0xd3 :
                    case 0xd4 :
                    case 0xd5 :
                    case 0xd6 :
                    case 0xd7 :     // RST7
                    case 0x01 :     // TEM
                    break;

                    case 0xd8 :     // SOI (has already been checked, there should not be a second one)
                    case 0x00 :     // marker is invalid, we should stop now
                        bScanFailure = sal_True;
                    break;

                    case 0xd9 :     // EOI
                        bScanFinished = sal_True;
                    break;

                    // per default we assume marker segments conaining a length parameter
                    default :
                    {
                        sal_uInt16 nLength;
                        rStm >> nLength;

                        if ( nLength < 2 )
                            bScanFailure = sal_True;
                        else
                        {
                            sal_uInt32 nNextMarkerPos = rStm.Tell() + nLength - 2;
                            switch( nMarker )
                            {
                                case 0xe0 : // APP0 Marker
                                {
                                    if ( nLength == 16 )
                                    {
                                        sal_Int32 nIdentifier;
                                        rStm >> nIdentifier;
                                        if ( nIdentifier == 0x4a464946 )    // JFIF Identifier
                                        {
                                            sal_uInt8   nStringTerminator;
                                            sal_uInt8   nMajorRevision;
                                            sal_uInt8   nMinorRevision;
                                            sal_uInt8   nUnits;
                                            sal_uInt16  nHorizontalResolution;
                                            sal_uInt16  nVerticalResolution;
                                            sal_uInt8   nHorzThumbnailPixelCount;
                                            sal_uInt8   nVertThumbnailPixelCount;

                                            rStm >> nStringTerminator
                                                 >> nMajorRevision
                                                 >> nMinorRevision
                                                 >> nUnits
                                                 >> nHorizontalResolution
                                                 >> nVerticalResolution
                                                 >> nHorzThumbnailPixelCount
                                                 >> nVertThumbnailPixelCount;

                                            // setting the logical size
                                            if ( nUnits && nHorizontalResolution && nVerticalResolution )
                                            {
                                                MapMode aMap;
                                                aMap.SetMapUnit( nUnits == 1 ? MAP_INCH : MAP_CM );
                                                aMap.SetScaleX( Fraction( 1, nHorizontalResolution ) );
                                                aMap.SetScaleY( Fraction( 1, nVerticalResolution ) );
                                                aLogSize = OutputDevice::LogicToLogic( aPixSize, aMap, MapMode( MAP_100TH_MM ) );
                                            }
                                        }
                                    }
                                }
                                break;

                                // Start of Frame Markers
                                case 0xc0 : // SOF0
                                case 0xc1 : // SOF1
                                case 0xc2 : // SOF2
                                case 0xc3 : // SOF3
                                case 0xc5 : // SOF5
                                case 0xc6 : // SOF6
                                case 0xc7 : // SOF7
                                case 0xc9 : // SOF9
                                case 0xca : // SOF10
                                case 0xcb : // SOF11
                                case 0xcd : // SOF13
                                case 0xce : // SOF14
                                case 0xcf : // SOF15
                                {
                                    sal_uInt8   nSamplePrecision;
                                    sal_uInt16  nNumberOfLines;
                                    sal_uInt16  nSamplesPerLine;
                                    sal_uInt8   nNumberOfImageComponents;
                                    sal_uInt8   nComponentsIdentifier;
                                    sal_uInt8   nHorizontalSamplingFactor;
                                    sal_uInt8   nVerticalSamplingFactor;
                                    sal_uInt8   nQuantizationTableDestinationSelector;
                                    rStm >> nSamplePrecision
                                         >> nNumberOfLines
                                         >> nSamplesPerLine
                                         >> nNumberOfImageComponents
                                         >> nComponentsIdentifier
                                         >> nHorizontalSamplingFactor
                                         >> nQuantizationTableDestinationSelector;
                                    nVerticalSamplingFactor = nHorizontalSamplingFactor & 0xf;
                                    nHorizontalSamplingFactor >>= 4;

                                    aPixSize.Height() = nNumberOfLines;
                                    aPixSize.Width() = nSamplesPerLine;
                                    nBitsPerPixel = ( nNumberOfImageComponents == 3 ? 24 : nNumberOfImageComponents == 1 ? 8 : 0 );
                                    nPlanes = 1;

                                    bScanFinished = sal_True;
                                }
                                break;
                            }
                            rStm.Seek( nNextMarkerPos );
                        }
                    }
                    break;
                }
            }
            rStm.SetError( nError );
        }
    }
    rStm.Seek( nStmPos );
    return bRet;
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectPCD( SvStream& rStm, BOOL )
{
    BOOL    bRet = FALSE;

    sal_Int32 nStmPos = rStm.Tell();
    rStm.SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );

    UINT32  nTemp32;
    UINT16  nTemp16;
    BYTE    cByte;

    rStm.SeekRel( 2048 );
    rStm >> nTemp32;
    rStm >> nTemp16;
    rStm >> cByte;

    if ( ( nTemp32 == 0x5f444350 ) &&
         ( nTemp16 == 0x5049 ) &&
         ( cByte == 0x49 ) )
    {
        nFormat = GFF_PCD;
        bRet = TRUE;
    }
    rStm.Seek( nStmPos );
    return bRet;
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectPCX( SvStream& rStm, BOOL bExtendedInfo )
{
    // ! Because 0x0a can be interpreted as LF too ...
    // we cant be shure that this special sign represent a PCX file only.
    // Every Ascii file is possible here :-(
    // We must detect the whole header.
    bExtendedInfo = TRUE;

    BOOL    bRet = FALSE;
    BYTE    cByte;

    sal_Int32 nStmPos = rStm.Tell();
    rStm.SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
    rStm >> cByte;

    if ( cByte == 0x0a )
    {
        nFormat = GFF_PCX;
        bRet = TRUE;

        if ( bExtendedInfo )
        {
            UINT16  nTemp16;
            USHORT  nXmin;
            USHORT  nXmax;
            USHORT  nYmin;
            USHORT  nYmax;
            USHORT  nDPIx;
            USHORT  nDPIy;


            rStm.SeekRel( 1 );

            // Kompression lesen
            rStm >> cByte;
            bCompressed = ( cByte > 0 );

            bRet = (cByte==0 || cByte ==1);

            // Bits/Pixel lesen
            rStm >> cByte;
            nBitsPerPixel = cByte;

            // Bildabmessungen
            rStm >> nTemp16;
            nXmin = nTemp16;
            rStm >> nTemp16;
            nYmin = nTemp16;
            rStm >> nTemp16;
            nXmax = nTemp16;
            rStm >> nTemp16;
            nYmax = nTemp16;

            aPixSize.Width() = nXmax - nXmin + 1;
            aPixSize.Height() = nYmax - nYmin + 1;

            // Aufloesung
            rStm >> nTemp16;
            nDPIx = nTemp16;
            rStm >> nTemp16;
            nDPIy = nTemp16;

            // logische Groesse setzen
            MapMode aMap( MAP_INCH, Point(),
                          Fraction( 1, nDPIx ), Fraction( 1, nDPIy ) );
            aLogSize = OutputDevice::LogicToLogic( aPixSize, aMap,
                                                   MapMode( MAP_100TH_MM ) );


            // Anzahl Farbebenen
            rStm.SeekRel( 49 );
            rStm >> cByte;
            nPlanes = cByte;

            bRet = (nPlanes<=4);
        }
    }

    rStm.Seek( nStmPos );
    return bRet;
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectPNG( SvStream& rStm, BOOL bExtendedInfo )
{
    UINT32  nTemp32;
    BOOL    bRet = FALSE;

    sal_Int32 nStmPos = rStm.Tell();
    rStm.SetNumberFormatInt( NUMBERFORMAT_INT_BIGENDIAN );
    rStm >> nTemp32;

    if ( nTemp32 == 0x89504e47 )
    {
        rStm >> nTemp32;
        if ( nTemp32 == 0x0d0a1a0a )
        {
            nFormat = GFF_PNG;
            bRet = TRUE;

            if ( bExtendedInfo )
            {
                BYTE cByte;

                // IHDR-Chunk
                rStm.SeekRel( 8 );

                // Breite einlesen
                rStm >> nTemp32;
                aPixSize.Width() = nTemp32;

                // Hoehe einlesen
                rStm >> nTemp32;
                aPixSize.Height() = nTemp32;

                // Bits/Pixel einlesen
                rStm >> cByte;
                nBitsPerPixel = cByte;

                // Planes immer 1;
                // Kompression immer
                nPlanes = 1;
                bCompressed = TRUE;

                UINT32  nLen32;

                rStm.SeekRel( 8 );

                // so lange ueberlesen, bis wir den pHYs-Chunk haben oder
                // den Anfang der Bilddaten
                rStm >> nLen32;
                rStm >> nTemp32;
                while( ( nTemp32 != 0x70485973 ) && ( nTemp32 != 0x49444154 ) )
                {
                    rStm.SeekRel( 4 + nLen32 );
                    rStm >> nLen32;
                    rStm >> nTemp32;
                }

                if ( nTemp32 == 0x70485973 )
                {
                    ULONG   nXRes;
                    ULONG   nYRes;

                    // horizontale Aufloesung
                    rStm >> nTemp32;
                    nXRes = nTemp32;

                    // vertikale Aufloesung
                    rStm >> nTemp32;
                    nYRes = nTemp32;

                    // Unit einlesen
                    rStm >> cByte;

                    if ( cByte )
                    {
                        if ( nXRes )
                            aLogSize.Width() = ( aPixSize.Width() * 100000 ) /
                                               nTemp32;

                        if ( nYRes )
                            aLogSize.Height() = ( aPixSize.Height() * 100000 ) /
                                                nTemp32;
                    }
                }
            }
        }
    }
    rStm.Seek( nStmPos );
    return bRet;
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectTIF( SvStream& rStm, BOOL bExtendedInfo )
{
    BOOL    bDetectOk = FALSE;
    BOOL    bRet = FALSE;
    BYTE    cByte1;
    BYTE    cByte2;

    sal_Int32 nStmPos = rStm.Tell();
    rStm >> cByte1;
    rStm >> cByte2;
    if ( cByte1 == cByte2 )
    {
        if ( cByte1 == 0x49 )
        {
            rStm.SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
            bDetectOk = TRUE;
        }
        else if ( cByte1 == 0x4d )
        {
            rStm.SetNumberFormatInt( NUMBERFORMAT_INT_BIGENDIAN );
            bDetectOk = TRUE;
        }

        if ( bDetectOk )
        {
            UINT16  nTemp16;

            rStm >> nTemp16;
            if ( nTemp16 == 0x2a )
            {
                nFormat = GFF_TIF;
                bRet = TRUE;

                if ( bExtendedInfo )
                {
                    ULONG   nCount;
                    ULONG   nMax = DATA_SIZE - 48;
                    UINT32  nTemp32;
                    BOOL    bOk = FALSE;

                    // Offset des ersten IFD einlesen
                    rStm >> nTemp32;
                    rStm.SeekRel( ( nCount = ( nTemp32 + 2 ) ) - 0x08 );

                    if ( nCount < nMax )
                    {
                        // Tag's lesen, bis wir auf Tag256 ( Width ) treffen
                        // nicht mehr Bytes als DATA_SIZE lesen
                        rStm >> nTemp16;
                        while ( nTemp16 != 256 )
                        {
                            bOk = nCount < nMax;
                            if ( !bOk )
                            {
                                break;
                            }
                            rStm.SeekRel( 10 );
                            rStm >> nTemp16;
                            nCount += 12;
                        }

                        if ( bOk )
                        {
                            // Breite lesen
                            rStm >> nTemp16;
                            rStm.SeekRel( 4 );
                            if ( nTemp16 == 3 )
                            {
                                rStm >> nTemp16;
                                aPixSize.Width() = nTemp16;
                                rStm.SeekRel( 2 );
                            }
                            else
                            {
                                rStm >> nTemp32;
                                aPixSize.Width() = nTemp32;
                            }
                            nCount += 12;

                            // Hoehe lesen
                            rStm.SeekRel( 2 );
                            rStm >> nTemp16;
                            rStm.SeekRel( 4 );
                            if ( nTemp16 == 3 )
                            {
                                rStm >> nTemp16;
                                aPixSize.Height() = nTemp16;
                                rStm.SeekRel( 2 );
                            }
                            else
                            {
                                rStm >> nTemp32;
                                aPixSize.Height() = nTemp32;
                            }
                            nCount += 12;

                            // ggf. Bits/Pixel lesen
                            rStm >> nTemp16;
                            if ( nTemp16 == 258 )
                            {
                                rStm.SeekRel( 6 );
                                rStm >> nTemp16;
                                nBitsPerPixel = nTemp16;
                                rStm.SeekRel( 2 );
                                nCount += 12;
                            }
                            else
                                rStm.SeekRel( -2 );

                            // ggf. Compression lesen
                            rStm >> nTemp16;
                            if ( nTemp16 == 259 )
                            {
                                rStm.SeekRel( 6 );
                                rStm >> nTemp16;
                                bCompressed = ( nTemp16 > 1 );
                                rStm.SeekRel( 2 );
                                nCount += 12;
                            }
                            else
                                rStm.SeekRel( -2 );
                        }
                    }
                }
            }
        }
    }
    rStm.Seek( nStmPos );
    return bRet;
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectXBM( SvStream&, BOOL )
{
    BOOL bRet = aPathExt.CompareToAscii( "xbm", 3 ) == COMPARE_EQUAL;
    if (bRet)
        nFormat = GFF_XBM;

    return bRet;
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectXPM( SvStream&, BOOL )
{
    BOOL bRet = aPathExt.CompareToAscii( "xpm", 3 ) == COMPARE_EQUAL;
    if (bRet)
        nFormat = GFF_XPM;

    return bRet;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectPBM( SvStream& rStm, BOOL )
{
    BOOL bRet = FALSE;

    // erst auf Datei Extension pruefen, da diese aussagekraeftiger ist
    // als die 2 ID Bytes

    if ( aPathExt.CompareToAscii( "pbm", 3 ) == COMPARE_EQUAL )
        bRet = TRUE;
    else
    {
        sal_Int32 nStmPos = rStm.Tell();
        BYTE    nFirst, nSecond;
        rStm >> nFirst >> nSecond;
        if ( nFirst == 'P' && ( ( nSecond == '1' ) || ( nSecond == '4' ) ) )
            bRet = TRUE;
        rStm.Seek( nStmPos );
    }

    if ( bRet )
        nFormat = GFF_PBM;

    return bRet;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectPGM( SvStream& rStm, BOOL )
{
    BOOL bRet = FALSE;

    if ( aPathExt.CompareToAscii( "pgm", 3 ) == COMPARE_EQUAL )
        bRet = TRUE;
    else
    {
        BYTE nFirst, nSecond;
        sal_Int32 nStmPos = rStm.Tell();
        rStm >> nFirst >> nSecond;
        if ( nFirst == 'P' && ( ( nSecond == '2' ) || ( nSecond == '5' ) ) )
            bRet = TRUE;
        rStm.Seek( nStmPos );
    }

    if ( bRet )
        nFormat = GFF_PGM;

    return bRet;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectPPM( SvStream& rStm, BOOL )
{
    BOOL bRet = FALSE;

    if ( aPathExt.CompareToAscii( "ppm", 3 ) == COMPARE_EQUAL )
        bRet = TRUE;
    else
    {
        BYTE    nFirst, nSecond;
        sal_Int32 nStmPos = rStm.Tell();
        rStm >> nFirst >> nSecond;
        if ( nFirst == 'P' && ( ( nSecond == '3' ) || ( nSecond == '6' ) ) )
            bRet = TRUE;
        rStm.Seek( nStmPos );
    }

    if ( bRet )
        nFormat = GFF_PPM;

    return bRet;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectRAS( SvStream& rStm, BOOL )
{
    UINT32 nMagicNumber;
    BOOL bRet = FALSE;
    sal_Int32 nStmPos = rStm.Tell();
    rStm.SetNumberFormatInt( NUMBERFORMAT_INT_BIGENDIAN );
    rStm >> nMagicNumber;
    if ( nMagicNumber == 0x59a66a95 )
    {
        nFormat = GFF_RAS;
        bRet = TRUE;
    }
    rStm.Seek( nStmPos );
    return bRet;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectTGA( SvStream&, BOOL )
{
    BOOL bRet = aPathExt.CompareToAscii( "tga", 3 ) == COMPARE_EQUAL;
    if (bRet)
        nFormat = GFF_TGA;

    return bRet;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectPSD( SvStream& rStm, BOOL bExtendedInfo )
{
    BOOL bRet = FALSE;

    UINT32  nMagicNumber;
    sal_Int32 nStmPos = rStm.Tell();
    rStm.SetNumberFormatInt( NUMBERFORMAT_INT_BIGENDIAN );
    rStm >> nMagicNumber;
    if ( nMagicNumber == 0x38425053 )
    {
        UINT16 nVersion;
        rStm >> nVersion;
        if ( nVersion == 1 )
        {
            bRet = TRUE;
            if ( bExtendedInfo )
            {
                UINT16 nChannels;
                UINT32 nRows;
                UINT32 nColumns;
                UINT16 nDepth;
                UINT16 nMode;
                rStm.SeekRel( 6 );  // Pad
                rStm >> nChannels >> nRows >> nColumns >> nDepth >> nMode;
                if ( ( nDepth == 1 ) || ( nDepth == 8 ) || ( nDepth == 16 ) )
                {
                    nBitsPerPixel = ( nDepth == 16 ) ? 8 : nDepth;
                    switch ( nChannels )
                    {
                        case 4 :
                        case 3 :
                            nBitsPerPixel = 24;
                        case 2 :
                        case 1 :
                            aPixSize.Width() = nColumns;
                            aPixSize.Height() = nRows;
                        break;
                        default:
                            bRet = FALSE;
                    }
                }
                else
                    bRet = FALSE;
            }
        }
    }

    if ( bRet )
        nFormat = GFF_PSD;
    rStm.Seek( nStmPos );
    return bRet;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectEPS( SvStream& rStm, BOOL )
{
    // es wird die EPS mit Vorschaubild Variante und die Extensionuebereinstimmung
    // geprueft

    sal_uInt32  nFirstLong;
    sal_uInt8   nFirstBytes[20];
    BOOL        bRet = FALSE;

    sal_Int32 nStmPos = rStm.Tell();
    rStm.SetNumberFormatInt( NUMBERFORMAT_INT_BIGENDIAN );
    rStm >> nFirstLong;
    rStm.SeekRel( -4 );
    rStm.Read( &nFirstBytes, 20 );

    if ( ( nFirstLong == 0xC5D0D3C6 ) || ( aPathExt.CompareToAscii( "eps", 3 ) == COMPARE_EQUAL ) ||
        ( ImplSearchEntry( nFirstBytes, (sal_uInt8*)"%!PS-Adobe", 10, 10 )
            && ImplSearchEntry( &nFirstBytes[15], (sal_uInt8*)"EPS", 3, 3 ) ) )
    {
        nFormat = GFF_EPS;
        bRet = TRUE;
    }
    rStm.Seek( nStmPos );
    return bRet;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectDXF( SvStream&, BOOL )
{
    BOOL bRet = aPathExt.CompareToAscii( "dxf", 3 ) == COMPARE_EQUAL;
    if (bRet)
        nFormat = GFF_DXF;

    return bRet;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectMET( SvStream&, BOOL )
{
    BOOL bRet = aPathExt.CompareToAscii( "met", 3 ) == COMPARE_EQUAL;
    if (bRet)
        nFormat = GFF_MET;

    return bRet;
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectPCT( SvStream& rStm, BOOL )
{
    BOOL bRet = aPathExt.CompareToAscii( "pct", 3 ) == COMPARE_EQUAL;
    if (bRet)
        nFormat = GFF_PCT;
    else
    {
        sal_Int32 nStmPos = rStm.Tell();

        BYTE sBuf[4];

        rStm.SeekRel( 522 );
        rStm.Read( sBuf, 3 );

        if( !rStm.GetError() )
        {
            if ( ( sBuf[0] == 0x00 ) && ( sBuf[1] == 0x11 ) &&
                 ( ( sBuf[2] == 0x01 ) || ( sBuf[2] == 0x02 ) ) )
            {
                bRet = TRUE;
                nFormat = GFF_PCT;
            }
        }
        rStm.Seek( nStmPos );
    }

    return bRet;
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectSGF( SvStream& rStm, BOOL )
{
    BOOL bRet = FALSE;
    if( aPathExt.CompareToAscii( "sgf", 3 ) == COMPARE_EQUAL )
        bRet = TRUE;
    else
    {
        sal_Int32 nStmPos = rStm.Tell();

        BYTE nFirst, nSecond;

        rStm >> nFirst >> nSecond;

        if( nFirst == 'J' && nSecond == 'J' )
            bRet = TRUE;

        rStm.Seek( nStmPos );
    }

    if( bRet )
        nFormat = GFF_SGF;

    return bRet;
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectSGV( SvStream&, BOOL )
{
    BOOL bRet = aPathExt.CompareToAscii( "sgv", 3 ) == COMPARE_EQUAL;
    if (bRet)
        nFormat = GFF_SGV;

    return bRet;
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectSVM( SvStream& rStm, BOOL bExtendedInfo )
{
    UINT32  n32;
    BOOL    bRet = FALSE;
    BYTE    cByte;

    sal_Int32 nStmPos = rStm.Tell();
    rStm.SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
    rStm >> n32;
    if ( n32 == 0x44475653 )
    {
        rStm >> cByte;
        if ( cByte == 0x49 )
        {
            nFormat = GFF_SVM;
            bRet = TRUE;

            if ( bExtendedInfo )
            {
                UINT32  nTemp32;
                UINT16  nTemp16;

                rStm.SeekRel( 0x04 );

                // Breite auslesen
                rStm >> nTemp32;
                aLogSize.Width() = nTemp32;

                // Hoehe auslesen
                rStm >> nTemp32;
                aLogSize.Height() = nTemp32;

                // Map-Unit auslesen und PrefSize ermitteln
                rStm >> nTemp16;
                aLogSize = OutputDevice::LogicToLogic( aLogSize,
                                                       MapMode( (MapUnit) nTemp16 ),
                                                       MapMode( MAP_100TH_MM ) );
            }
        }
    }
    else
    {
        rStm.SeekRel( -4L );
        rStm >> n32;

        if( n32 == 0x4D4C4356 )
        {
            UINT16 nTmp16;

            rStm >> nTmp16;

            if( nTmp16 == 0x4654 )
            {
                nFormat = GFF_SVM;
                bRet = TRUE;

                if( bExtendedInfo )
                {
                    MapMode aMapMode;

                    rStm.SeekRel( 0x06 );
                    rStm >> aMapMode;
                    rStm >> aLogSize;
                    aLogSize = OutputDevice::LogicToLogic( aLogSize, aMapMode, MapMode( MAP_100TH_MM ) );
                }
            }
        }
    }
    rStm.Seek( nStmPos );
    return bRet;
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectWMF( SvStream&, BOOL )
{
    BOOL bRet = aPathExt.CompareToAscii( "wmf",3 ) == COMPARE_EQUAL;
    if (bRet)
        nFormat = GFF_WMF;

    return bRet;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL GraphicDescriptor::ImpDetectEMF( SvStream&, BOOL )
{
    BOOL bRet = aPathExt.CompareToAscii( "emf", 3 ) == COMPARE_EQUAL;
    if (bRet)
        nFormat = GFF_EMF;

    return bRet;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

String GraphicDescriptor::GetImportFormatShortName( sal_uInt16 nFormat )
{
    ByteString          aKeyName;

    switch( nFormat )
    {
        case( GFF_BMP ) :   aKeyName = "bmp";   break;
        case( GFF_GIF ) :   aKeyName = "gif";   break;
        case( GFF_JPG ) :   aKeyName = "jpg";   break;
        case( GFF_PCD ) :   aKeyName = "pcd";   break;
        case( GFF_PCX ) :   aKeyName = "pcx";   break;
        case( GFF_PNG ) :   aKeyName = "png";   break;
        case( GFF_XBM ) :   aKeyName = "xbm";   break;
        case( GFF_XPM ) :   aKeyName = "xpm";   break;
        case( GFF_PBM ) :   aKeyName = "pbm";   break;
        case( GFF_PGM ) :   aKeyName = "pgm";   break;
        case( GFF_PPM ) :   aKeyName = "ppm";   break;
        case( GFF_RAS ) :   aKeyName = "ras";   break;
        case( GFF_TGA ) :   aKeyName = "tga";   break;
        case( GFF_PSD ) :   aKeyName = "psd";   break;
        case( GFF_EPS ) :   aKeyName = "eps";   break;
        case( GFF_TIF ) :   aKeyName = "tif";   break;
        case( GFF_DXF ) :   aKeyName = "dxf";   break;
        case( GFF_MET ) :   aKeyName = "met";   break;
        case( GFF_PCT ) :   aKeyName = "pct";   break;
        case( GFF_SGF ) :   aKeyName = "sgf";   break;
        case( GFF_SGV ) :   aKeyName = "sgv";   break;
        case( GFF_SVM ) :   aKeyName = "svm";   break;
        case( GFF_WMF ) :   aKeyName = "wmf";   break;
        case( GFF_EMF ) :   aKeyName = "emf";   break;
    }

    return String( aKeyName, RTL_TEXTENCODING_ASCII_US );
}


