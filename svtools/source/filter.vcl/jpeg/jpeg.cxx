/*************************************************************************
 *
 *  $RCSfile: jpeg.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: avy $ $Date: 2001-08-21 14:24:13 $
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

extern "C"
{
    #include "jpeg/jpeg.h"
}

#define _JPEGPRIVATE

#include <tools/new.hxx>
#include <vcl/bmpacc.hxx>
#include "jpeg.hxx"
#include "FilterConfigItem.hxx"
#include "filter.hxx"

// -----------
// - Defines -
// -----------

#define JPEGMINREAD 512

// -------------
// - (C-Calls) -
// -------------

extern "C" void* JPEGMalloc( size_t nSize )
{
    return (void*) new BYTE[ nSize ];
}

// ------------------------------------------------------------------------

extern "C" void JPEGFree( void* pBuf )
{
    delete[] (BYTE*) pBuf;
}

// ------------------------------------------------------------------------

extern "C" long StreamRead( void* pIStm, void* pBuffer, long nBufferSize )
{
    SvStream*   pSvStm = (SvStream*) pIStm;
    long        nRead;

    if( pSvStm->GetError() != ERRCODE_IO_PENDING )
    {
        long nActPos = pSvStm->Tell();

        nRead = (long) pSvStm->Read( pBuffer, nBufferSize );

        if( pSvStm->GetError() == ERRCODE_IO_PENDING )
        {
            nRead = 0;

            // Damit wir wieder an die alte Position
            // seeken koennen, setzen wir den Error temp.zurueck
            pSvStm->ResetError();
            pSvStm->Seek( nActPos );
            pSvStm->SetError( ERRCODE_IO_PENDING );
        }
    }
    else
        nRead = 0;

    return nRead;
}

// ------------------------------------------------------------------------

extern "C" long StreamWrite( void* pOStm, void* pBuffer, long nBufferSize )
{
    return (long) ( (SvStream*) pOStm )->Write( pBuffer, nBufferSize );
}

// ------------------------------------------------------------------------

extern "C" void* CreateBitmap( void* pJPEGReader, void* pJPEGCreateBitmapParam )
{
    return ( (JPEGReader*) pJPEGReader )->CreateBitmap( pJPEGCreateBitmapParam );
}

// ------------------------------------------------------------------------

extern "C" void* GetScanline( void* pJPEGWriter, long nY )
{
    return ( (JPEGWriter*) pJPEGWriter )->GetScanline( nY );
}

// ------------------------------------------------------------------------

extern "C" long JPEGCallback( void* pCallbackData, long nPercent )
{
/*
    MyCallbackHandler* pH = (MyCallbackHandler*) pCallbackData;

    if ( pH->pCallback )
    {
        return (short) (pH->pCallback) ( pH->pCallerData,
            (USHORT) (pH->nMinPercent+(pH->nMaxPercent-pH->nMinPercent) * nPercent / 100 ) );
    }
*/

    return 0L;
}

// --------------
// - JPEGReader -
// --------------

JPEGReader::JPEGReader( SvStream& rStm, void* pCallData, sal_Bool bSetLS ) :
        rIStm           ( rStm ),
        pAcc            ( NULL ),
        pAcc1           ( NULL ),
        pBuffer         ( NULL ),
        nLastPos        ( rStm.Tell() ),
        nLastLines      ( 0 ),
        bSetLogSize     ( bSetLS )
{
    maUpperName = String::CreateFromAscii( "SVIJPEG", 7 );
    nFormerPos = nLastPos;
}

// ------------------------------------------------------------------------

JPEGReader::~JPEGReader()
{
    if( pBuffer )
        SvMemFree( pBuffer );

    if( pAcc )
        aBmp.ReleaseAccess( pAcc );

    if( pAcc1 )
        aBmp1.ReleaseAccess( pAcc1 );
}

// ------------------------------------------------------------------------

void* JPEGReader::CreateBitmap( void* pParam )
{
    Size        aSize( ((JPEGCreateBitmapParam*)pParam)->nWidth,
                        ((JPEGCreateBitmapParam*)pParam)->nHeight );
    sal_Bool    bGray = ((JPEGCreateBitmapParam*)pParam)->bGray != 0;

    void* pBmpBuf = NULL;

    if( pAcc )
        aBmp.ReleaseAccess( pAcc );

    if( bGray )
    {
        BitmapPalette aGrayPal( 256 );

        for( USHORT n = 0; n < 256; n++ )
        {
            const BYTE cGray = (BYTE) n;
            aGrayPal[ n ] = BitmapColor( cGray, cGray, cGray );
        }

        aBmp = Bitmap( aSize, 8, &aGrayPal );
    }
    else
        aBmp = Bitmap( aSize, 24 );

    if ( bSetLogSize )
    {
        unsigned long nUnit = ((JPEGCreateBitmapParam*)pParam)->density_unit;
        if ( ( nUnit == 1 ) || ( nUnit == 2 ) )
        {
            Point       aEmptyPoint;
            Fraction    aFractX( 1, ((JPEGCreateBitmapParam*)pParam)->X_density );
            Fraction    aFractY( 1, ((JPEGCreateBitmapParam*)pParam)->Y_density );
            MapMode     aMapMode( nUnit == 1 ? MAP_INCH : MAP_CM, aEmptyPoint, aFractX, aFractY );
            Size        aPrefSize = OutputDevice::LogicToLogic( aSize, aMapMode, MAP_100TH_MM );

            aBmp.SetPrefSize( aPrefSize );
            aBmp.SetPrefMapMode( MapMode( MAP_100TH_MM ) );
        }
    }
    pAcc = aBmp.AcquireWriteAccess();

    if( pAcc )
    {
        long nAlignedWidth;

        const ULONG nFormat = pAcc->GetScanlineFormat();

        if( ( bGray && ( BMP_FORMAT_8BIT_PAL == nFormat ) ) ||
            ( !bGray && ( BMP_FORMAT_24BIT_TC_BGR == nFormat ) ) )
        {
            pBmpBuf = pAcc->GetBuffer();
            nAlignedWidth = pAcc->GetScanlineSize();
            ((JPEGCreateBitmapParam*)pParam)->bTopDown = pAcc->IsTopDown();
        }
        else
        {
            nAlignedWidth = AlignedWidth4Bytes( aSize.Width() * ( bGray ? 8 : 24 ) );
            ((JPEGCreateBitmapParam*)pParam)->bTopDown = TRUE;
            pBmpBuf = pBuffer = SvMemAlloc( nAlignedWidth * aSize.Height() );
        }
        ((JPEGCreateBitmapParam*)pParam)->nAlignedWidth = nAlignedWidth;
    }

    return pBmpBuf;
}

// ------------------------------------------------------------------------

void JPEGReader::FillBitmap()
{
    if( pBuffer && pAcc )
    {
        HPBYTE      pTmp;
        BitmapColor aColor;
        long        nAlignedWidth;
        long        nWidth = pAcc->Width();
        long        nHeight = pAcc->Height();

        if( pAcc->GetBitCount() == 8 )
        {
            BitmapColor* pCols = new BitmapColor[ 256 ];

            for( USHORT n = 0; n < 256; n++ )
            {
                const BYTE cGray = (BYTE) n;
                pCols[ n ] = pAcc->GetBestMatchingColor( BitmapColor( cGray, cGray, cGray ) );
            }

            nAlignedWidth = AlignedWidth4Bytes( pAcc->Width() * 8L );

            for( long nY = 0L; nY < nHeight; nY++ )
            {
                pTmp = (BYTE*) pBuffer + nY * nAlignedWidth;

                for( long nX = 0L; nX < nWidth; nX++ )
                    pAcc->SetPixel( nY, nX, pCols[ *pTmp++ ] );
            }

            delete[] pCols;
        }
        else
        {
            nAlignedWidth = AlignedWidth4Bytes( pAcc->Width() * 24L );

            for( long nY = 0L; nY < nHeight; nY++ )
            {
                pTmp = (BYTE*) pBuffer + nY * nAlignedWidth;

                for( long nX = 0L; nX < nWidth; nX++ )
                {
                    aColor.SetBlue( *pTmp++ );
                    aColor.SetGreen( *pTmp++ );
                    aColor.SetRed( *pTmp++ );
                    pAcc->SetPixel( nY, nX, aColor );
                }
            }
        }
    }
}

// ------------------------------------------------------------------------

Graphic JPEGReader::CreateIntermediateGraphic( const Bitmap& rBitmap, long nLines )
{
    Graphic     aGraphic;
    const Size  aSizePix( rBitmap.GetSizePixel() );

    if( !nLastLines )
    {
        if( pAcc1 )
            aBmp1.ReleaseAccess( pAcc1 );

        aBmp1 = Bitmap( rBitmap.GetSizePixel(), 1 );
        aBmp1.Erase( Color( COL_WHITE ) );
        pAcc1 = aBmp1.AcquireWriteAccess();
    }

    if( nLines && ( nLines < aSizePix.Height() ) )
    {
        if( pAcc1 )
        {
            const long nNewLines = nLines - nLastLines;

            if( nNewLines )
            {
                pAcc1->SetFillColor( Color( COL_BLACK ) );
                pAcc1->FillRect( Rectangle( Point( 0, nLastLines ),
                                            Size( pAcc1->Width(), nNewLines ) ) );
            }

            aBmp1.ReleaseAccess( pAcc1 );
            aGraphic = BitmapEx( rBitmap, aBmp1 );
            pAcc1 = aBmp1.AcquireWriteAccess();
        }
        else
            aGraphic = rBitmap;
    }
    else
        aGraphic = rBitmap;

    nLastLines = nLines;

    return aGraphic;
}

// ------------------------------------------------------------------------

ReadState JPEGReader::Read( Graphic& rGraphic )
{
    long        nEndPos;
    long        nLines;
    ReadState   eReadState;
    BOOL        bRet = FALSE;
    BYTE        cDummy;

    // sehen, ob wir _alles_ lesen koennen
    rIStm.Seek( STREAM_SEEK_TO_END );
    rIStm >> cDummy;
    nEndPos = rIStm.Tell();

    // falls wir nicht alles lesen koennen, gucken wir,
    // ob min. JPEGMINREAD Bytes gelesen werden koennen
    if( rIStm.GetError() == ERRCODE_IO_PENDING )
    {
        rIStm.ResetError();
        if( ( nEndPos  - nFormerPos ) < JPEGMINREAD )
        {
            rIStm.Seek( nLastPos );
            return JPEGREAD_NEED_MORE;
        }
    }

    // an Anfang springen
    rIStm.Seek( nLastPos );

    // (Teil-) Bild einlesen
    ReadJPEG( this, &rIStm, &nLines );

    if( pAcc )
    {
        if( pBuffer )
        {
            FillBitmap();
            SvMemFree( pBuffer );
            pBuffer = NULL;
        }

        aBmp.ReleaseAccess( pAcc );
        pAcc = NULL;

        if( rIStm.GetError() == ERRCODE_IO_PENDING )
            rGraphic = CreateIntermediateGraphic( aBmp, nLines );
        else
            rGraphic = aBmp;

        bRet = TRUE;
    }
    else if( rIStm.GetError() == ERRCODE_IO_PENDING )
        bRet = TRUE;

    // Status setzen ( Pending hat immer Vorrang )
    if( rIStm.GetError() == ERRCODE_IO_PENDING )
    {
        eReadState = JPEGREAD_NEED_MORE;
        rIStm.ResetError();
        nFormerPos = rIStm.Tell();
    }
    else
    {
        if( bRet )
            eReadState = JPEGREAD_OK;
        else
            eReadState = JPEGREAD_ERROR;
    }

    return eReadState;
}


// --------------
// - JPEGWriter -
// --------------

JPEGWriter::JPEGWriter( SvStream& rStm, PFilterCallback pClb, void* pData ) :
        rOStm       ( rStm ),
        pAcc        ( NULL ),
        pBuffer     ( NULL ),
        pCallback   ( pClb ),
        pCallerData ( pData )
{
}

// ------------------------------------------------------------------------

void* JPEGWriter::GetScanline( long nY )
{
    void* pScanline = NULL;

    if( pAcc )
    {
        if( bNative )
            pScanline = pAcc->GetScanline( nY );
        else if( pBuffer )
        {
            BitmapColor aColor;
            long        nWidth = pAcc->Width();
            BYTE*       pTmp = pBuffer;

            if( pAcc->HasPalette() )
            {
                for( long nX = 0L; nX < nWidth; nX++ )
                {
                    aColor = pAcc->GetPaletteColor( (BYTE) pAcc->GetPixel( nY, nX ) );
                    *pTmp++ = aColor.GetBlue();
                    *pTmp++ = aColor.GetGreen();
                    *pTmp++ = aColor.GetRed();
                }
            }
            else
            {
                for( long nX = 0L; nX < nWidth; nX++ )
                {
                    aColor = pAcc->GetPixel( nY, nX );
                    *pTmp++ = aColor.GetBlue();
                    *pTmp++ = aColor.GetGreen();
                    *pTmp++ = aColor.GetRed();
                }
            }

            pScanline = pBuffer;
        }
    }

    return pScanline;
}

// ------------------------------------------------------------------------

BOOL JPEGWriter::Write( const Graphic& rGraphic, sal_Bool bIgnoreOptions )
{
    BOOL bRet;

    aBmp = rGraphic.GetBitmap();
    pAcc = aBmp.AcquireReadAccess();

    if( pAcc )
    {
        sal_Int32 nQuality = 75;
        if ( !bIgnoreOptions )
        {
            FilterConfigItem aConfigItem( String( RTL_CONSTASCII_USTRINGPARAM( "Office.Common/Filter/Graphic/Export/JPG" ) ) );
            nQuality = aConfigItem.ReadInt32( String( RTL_CONSTASCII_USTRINGPARAM( "Quality" ) ), 75 );
        }
        bNative = ( pAcc->GetScanlineFormat() == BMP_FORMAT_24BIT_TC_BGR );

        if( !bNative )
            pBuffer = new BYTE[ AlignedWidth4Bytes( pAcc->Width() * 24L ) ];

        bRet = (BOOL) WriteJPEG( this, &rOStm, pAcc->Width(), pAcc->Height(), nQuality, NULL );

//      MyCallbackHandler   aCbH;
//      aCbH.pCallback = pCallback;
//      aCbH.pCallerData = pCallerData;
//      aCbH.nMinPercent = 0;
//      aCbH.nMaxPercent = 99;
//      bRet = (BOOL) WriteJPEG( this, &rOStm, pAcc->Width(), pAcc->Height(), nQuality, &aCbH );

        delete[] pBuffer;
        pBuffer = NULL;

        aBmp.ReleaseAccess( pAcc );
        pAcc = NULL;
    }
    else
        bRet = FALSE;

    return bRet;
}

// --------------
// - ImportJPEG -
// --------------

BOOL ImportJPEG( SvStream& rStm, Graphic& rGraphic, void* pCallerData, sal_Int32 nImportFlags )
{
    JPEGReader* pJPEGReader = (JPEGReader*) rGraphic.GetContext();
    ReadState   eReadState;
    BOOL        bRet = TRUE;

    if( !pJPEGReader )
        pJPEGReader = new JPEGReader( rStm, pCallerData, ( nImportFlags & GRFILTER_I_FLAGS_SET_LOGSIZE_FOR_JPEG ) != 0 );

    rGraphic.SetContext( NULL );
    eReadState = pJPEGReader->Read( rGraphic );

    if( eReadState == JPEGREAD_ERROR )
    {
        bRet = FALSE;
        delete pJPEGReader;
    }
    else if( eReadState == JPEGREAD_OK )
        delete pJPEGReader;
    else
        rGraphic.SetContext( pJPEGReader );

    return bRet;
}

// --------------
// - ExportJPEG -
// --------------

BOOL ExportJPEG( SvStream& rOStm, const Graphic& rGraphic,
                    PFilterCallback pCallback, void* pCallerData, sal_Bool bIgnoreOptions )
{
    JPEGWriter aJPEGWriter( rOStm, pCallback, pCallerData );
    return aJPEGWriter.Write( rGraphic, bIgnoreOptions );
}
