/*************************************************************************
 *
 *  $RCSfile: bmpcore.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: ka $ $Date: 2002-04-04 09:29:59 $
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

#include <tools/color.hxx>
#include <vcl/bmpacc.hxx>
#include "bmpcore.hxx"

#include <vector>
#include <algorithm>

#define FILETEST(FileEntry) ((FileEntry).Exists())

// --------------
// - BmpCreator -
// --------------

BmpCreator::BmpCreator()
{
}

// -----------------------------------------------------------------------------

BmpCreator::~BmpCreator()
{
}

// -----------------------------------------------------------------------------

void BmpCreator::Message( const String& rText, BYTE cExitCode )
{
}

// -----------------------------------------------------------------------

void BmpCreator::ImplCreate( SvStream& rStm, const DirEntry& rIn, const DirEntry& rOut, const String& rName, const LangInfo& rLang )
{
       const char*         pCollectPath = getenv( "BMP_COLLECT_PATH" );
    const char*         pResPath = getenv( "SOLARSRC" );
    const sal_uInt32    nOldPos = pSRS->Tell();

    if( pResPath && *pResPath )
    {
        ByteString              aLine;
        String                  aFileName, aInfo, aPrefix, aName( rName );
        String                  aString( String::CreateFromAscii( ByteString( pResPath ).GetBuffer() ) );
        const String            aResPath( ( DirEntry( aString ) += DirEntry( String( RTL_CONSTASCII_USTRINGPARAM( "res" ) ) ) ).GetFull() );
        SvFileStream            aOutStream;
        Bitmap                  aTotalBmp;
        DirEntry                aOutFile( rOut );
        DirEntry                aLocalPath( rIn + DirEntry( String( RTL_CONSTASCII_USTRINGPARAM( "x.bmp" ) ) ) );
        DirEntry                aLocalCollectPath;
        DirEntry                aGlobalPath( aResPath );
        DirEntry                aGlobalLangPath( aResPath );
        DirEntry                aGlobalCollectPath;
        ::std::vector< String > aNameVector;

        // get prefix for files
        if( ( aName.Len() >= 3 ) && ( aName.GetChar( 2 ) != '_' ) )
            aPrefix = String( aName, 0, 3 );
        else
            aPrefix = String( aName, 0, 2 );

        if( rLang.mnLangNum != 49 )
        {
            // add country id, if not german
            String aNumStr( String::CreateFromInt32( rLang.mnLangNum ) );

            if( aNumStr.Len() == 1 )
                aNumStr.Insert( '0', 0 );

            aName = DirEntry( aName ).GetBase();
            aName += aNumStr;
            aName += String( RTL_CONSTASCII_USTRINGPARAM( ".bmp" ) );
            aGlobalLangPath += DirEntry( ::rtl::OUString::createFromAscii( rLang.maLangDir ) );
        }

        // create output file name
        aOutFile += DirEntry( aName );

        // names are replaced later
        aGlobalLangPath += DirEntry( String( RTL_CONSTASCII_USTRINGPARAM( "x.bmp" ) ) );
        aGlobalPath += DirEntry( String( RTL_CONSTASCII_USTRINGPARAM( "x.bmp" ) ) );

        // get number of bitmaps
        while( aLine.Search( '}' ) == STRING_NOTFOUND )
        {
            if( !pSRS->ReadLine( aLine ) )
                break;

            aLine.EraseLeadingChars( ' ' );
            aLine.EraseLeadingChars( '\t' );
            aLine.EraseAllChars( ';' );

            if( aLine.IsNumericAscii() )
            {
                aString = aPrefix;

                if( atoi( aLine.GetBuffer() ) < 10000 )
                    aString += String::CreateFromInt32( 0 );

                aString += String( aLine.GetBuffer(), RTL_TEXTENCODING_UTF8 );
                aString += String( RTL_CONSTASCII_USTRINGPARAM( ".bmp" ) );

                aNameVector.push_back( aString );
            }
        }

        if( !aNameVector.size() )
            Message( String( RTL_CONSTASCII_USTRINGPARAM( "WARNING: No imagelist resource found: " ) ).Append( aString ), EXIT_MISSING_RESOURCE );
        else
        {
            // write info
            aInfo = String( RTL_CONSTASCII_USTRINGPARAM( "CREATING ImageList for language: " ) );
            aInfo += String( ::rtl::OUString::createFromAscii( rLang.maLangDir ) );
            aInfo += String( RTL_CONSTASCII_USTRINGPARAM( " [ " ) );
            aInfo += aLocalPath.GetPath().GetFull();
            aInfo += String( RTL_CONSTASCII_USTRINGPARAM( "; " ) );
            aInfo += aGlobalLangPath.GetPath().GetFull();

            if( aGlobalPath != aGlobalLangPath )
            {
                aInfo += String( RTL_CONSTASCII_USTRINGPARAM( "; " ) );
                aInfo += aGlobalPath.GetPath().GetFull();
            }

            aInfo += String( RTL_CONSTASCII_USTRINGPARAM( " ]" ) );
            Message( aInfo );

            if( pCollectPath )
            {
                String aLocalStr( aLocalPath.GetPath().GetFull() );
                aLocalStr.EraseLeadingChars( '/' );
                aLocalStr.SearchAndReplace( ':', '_' );
                aLocalCollectPath = DirEntry( String( pCollectPath, RTL_TEXTENCODING_ASCII_US ) );
                aLocalCollectPath += DirEntry( aLocalStr );
                aLocalCollectPath.MakeDir();

                String aGlobalStr( aGlobalPath.GetPath().GetFull() );
                aGlobalStr.EraseLeadingChars( '/' );
                aGlobalStr.SearchAndReplace( ':', '_' );
                aGlobalCollectPath = DirEntry( String( pCollectPath, RTL_TEXTENCODING_ASCII_US ) );
                aGlobalCollectPath += DirEntry( aGlobalStr );
                aGlobalCollectPath.MakeDir();

                if( !aLocalCollectPath.Exists() || !aGlobalCollectPath.Exists() )
                {
                    pCollectPath = NULL;
                    Message( String( RTL_CONSTASCII_USTRINGPARAM( "ERROR: couldn't create collect path" ) ), 0 );
                }
            }

            // create bit vector to hold flags for valid bitmaps
            ::std::bit_vector aValidBmpBitVector( aNameVector.size(), false );

            for( sal_uInt32 n = 0; n < aNameVector.size(); n++ )
            {
                Bitmap aBmp;

                aLocalPath.SetName( aString = aNameVector[ n ] );

                if( !FILETEST( aLocalPath ) )
                {
                    // Falls nicht deutsch, suchen wir zuerst im jeweiligen Sprach-Unterverz.
                    if( rLang.mnLangNum != 49 )
                    {
                        aGlobalLangPath.SetName( aString );

                        if ( !FILETEST( aGlobalLangPath ) )
                        {
                            aGlobalPath.SetName( aString );

                            if( !FILETEST( aGlobalPath ) )
                                aBmp = Bitmap();
                            else
                            {
                                SvFileStream aIStm( aFileName = aGlobalPath.GetFull(), STREAM_READ );
                                aIStm >> aBmp;
                            }
                        }
                        else
                        {
                            SvFileStream aIStm( aFileName = aGlobalLangPath.GetFull(), STREAM_READ );
                            aIStm >> aBmp;
                        }
                    }
                    else
                    {
                        aGlobalPath.SetName( aString );

                        if( !FILETEST( aGlobalPath ) )
                            aBmp = Bitmap();
                        else
                        {
                            SvFileStream aIStm( aFileName = aGlobalPath.GetFull(), STREAM_READ );
                            aIStm >> aBmp;
                        }
                    }

                    if( pCollectPath && !aBmp.IsEmpty() )
                    {
                        DirEntry aSrcPath( aFileName ), aDstPath( aGlobalCollectPath );
                        aSrcPath.CopyTo( aDstPath += aSrcPath.GetName(), FSYS_ACTION_COPYFILE );
                    }
                }
                else
                {
                    SvFileStream aIStm( aFileName = aLocalPath.GetFull(), STREAM_READ );
                    aIStm >> aBmp;
                    aIStm.Close();

                    if( pCollectPath && !aBmp.IsEmpty() )
                    {
                        DirEntry aSrcPath( aFileName ), aDstPath( aLocalCollectPath );
                        aSrcPath.CopyTo( aDstPath += aSrcPath.GetName(), FSYS_ACTION_COPYFILE );
                    }
                }

                const Size aSize( aBmp.GetSizePixel() );

                if( !aSize.Width() || !aSize.Height() )
                    Message( String( RTL_CONSTASCII_USTRINGPARAM( "WARNING: Bitmap is missing: " ) ).Append( aString )/*, EXIT_MISSING_BITMAP*/ );
                else
                {
                    if( aTotalBmp.IsEmpty() )
                    {
                        // first bitmap determines metrics of total bitmap
                        Size aTotalSize( aOneSize = aSize );
                        aTotalSize.Width() *= aNameVector.size();
                        aTotalBmp = Bitmap( aTotalSize, aBmp.GetBitCount() );
                    }

                    if( ( aSize.Width() > aOneSize.Width() ) || ( aSize.Height() > aOneSize.Height() ) )
                         Message( String( RTL_CONSTASCII_USTRINGPARAM( "ERROR: Different dimensions in file: " ) ).Append( aString )/*, EXIT_DIMENSIONERROR*/ );
                    else if( aBmp.GetBitCount() != aTotalBmp.GetBitCount() )
                         Message( String( RTL_CONSTASCII_USTRINGPARAM( "ERROR: Different color depth in file: ") ).Append( aString )/*, EXIT_COLORDEPTHERROR*/ );
                    else
                    {
                        Point           aPoint;
                        const Rectangle aDst( Point( aOneSize.Width() * n, 0L ), aSize );
                        const Rectangle aSrc( aPoint, aSize );

                        if( !aTotalBmp.IsEmpty() && !aBmp.IsEmpty() && !aDst.IsEmpty() && !aSrc.IsEmpty() )
                        {
                            aTotalBmp.CopyPixel( aDst, aSrc, &aBmp );
                            aValidBmpBitVector[ n ] = true;
                        }
                    }
                }
            }

            if( !aTotalBmp.IsEmpty() )
            {
                // do we have invalid bitmaps?
                if( ::std::find( aValidBmpBitVector.begin(), aValidBmpBitVector.end(), false ) != aValidBmpBitVector.end() )
                {
                    // replace invalid entries with a red cross
                    BitmapWriteAccess* pAcc = aTotalBmp.AcquireWriteAccess();

                    if( pAcc )
                    {
                        pAcc->SetLineColor( Color( COL_LIGHTGREEN ) );

                        for( sal_uInt32 n = 0; n < aValidBmpBitVector.size(); n++ )
                        {
                            if( !aValidBmpBitVector[ n ] )
                            {
                                const Rectangle aDst( Point( aOneSize.Width() * n, 0L ), aOneSize );

                                pAcc->DrawRect( aDst );
                                pAcc->DrawLine( aDst.TopLeft(), aDst.BottomRight() );
                                pAcc->DrawLine( aDst.TopRight(), aDst.BottomLeft() );
                            }
                        }

                        aTotalBmp.ReleaseAccess( pAcc );
                    }
                }

                // write output file
                const String aFile( aOutFile.GetFull() );

                aOutStream.Open( aFile, STREAM_WRITE | STREAM_TRUNC );

                if( !aOutStream.IsOpen() )
                    Message( String( RTL_CONSTASCII_USTRINGPARAM( "ERROR: Could not open output file: " ) ).Append( aFile ), EXIT_IOERROR );
                else
                {
                    aOutStream << aTotalBmp;

                    if( aOutStream.GetError() )
                        Message( String( RTL_CONSTASCII_USTRINGPARAM( "ERROR: Could not write to output file: " ) ).Append( aFile ), EXIT_IOERROR );
                    else
                        Message( String( RTL_CONSTASCII_USTRINGPARAM( "Successfully generated ImageList " ) ).Append( aFile ) );

                    aOutStream.Close();
                }
            }
            else
                Message( String( RTL_CONSTASCII_USTRINGPARAM( "ERROR: Could not generate  " ) ).Append( aOutFile.GetFull() ), EXIT_COMMONERROR );

            Message( ' ' );
        }
    }
    else
        Message( String( RTL_CONSTASCII_USTRINGPARAM( "ERROR: SOLARSRC environment variable not set!" ) ), EXIT_MISSING_SOLARSRC_ENV );

    pSRS->Seek( nOldPos );
}

// -----------------------------------------------------------------------------

void BmpCreator::Create( const String& rSRSName, const String& rInName,
                         const String& rOutName, const LangInfo& rLang )
{
    DirEntry    aFileName( rSRSName ), aInDir( rInName ), aOutDir( rOutName );
    BOOL        bDone = FALSE;

    aFileName.ToAbs();
    aInDir.ToAbs();
    aOutDir.ToAbs();

    pSRS = new SvFileStream ( aFileName.GetFull(), STREAM_STD_READ );

    if( pSRS->GetError() )
        Message( String( RTL_CONSTASCII_USTRINGPARAM( "ERROR: Kein SRS file!" ) ), EXIT_NOSRSFILE );
    else
    {
        String      aText;
        ByteString  aByteText;
        BOOL        bLangDep = FALSE;

        do
        {
            do
            {
                if (!pSRS->ReadLine(aByteText))
                    break;
            }
            while ( aByteText.Search( "ImageList" ) == STRING_NOTFOUND );

            do
            {
                if (!pSRS->ReadLine( aByteText ) )
                    break;
            }
            while ( aByteText.Search( "File" ) == STRING_NOTFOUND );
            aText = String::CreateFromAscii( aByteText.GetBuffer() );

            const String aName( aText.GetToken( 1, '"' ) );

            do
            {
                if( !bLangDep &&
                    aByteText.Search( "File" ) != STRING_NOTFOUND &&
                    aByteText.Search( '[' ) != STRING_NOTFOUND &&
                    aByteText.Search( ']' ) != STRING_NOTFOUND )
                {
                    bLangDep = TRUE;
                }

                if (!pSRS->ReadLine(aByteText))
                     break;
            }
            while (aByteText.Search( "IdList" ) == STRING_NOTFOUND );
            aText = String::CreateFromAscii( aByteText.GetBuffer() );

            // if image list is not language dependent, don't do anything for languages except german
            if( aText.Len() )
            {
                bDone = TRUE;
                 ImplCreate( *pSRS, aInDir, aOutDir, aName, rLang );
            }
            else if( ( rLang.mnLangNum != 49 ) && !bLangDep )
            {
                Message( String( RTL_CONSTASCII_USTRINGPARAM( "INFO: ImageList is not language dependent! Nothing to do for this language." ) ) );
                bDone = TRUE;
            }
        }
        while ( aText.Len() );
    }

    if( !bDone )
        Message( String( RTL_CONSTASCII_USTRINGPARAM( "ERROR: No ImageList found in SRS file!" ) ), EXIT_NOIMGLIST );

    delete pSRS;
}
