/*************************************************************************
 *
 *  $RCSfile: g2g.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: ka $ $Date: 2001-09-26 09:33:34 $
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

#include <cstdio>
#include <csignal>
#include <tools/fsys.hxx>
#include <tools/stream.hxx>
#include <vcl/svapp.hxx>
#include "filter.hxx"

#define EXIT_NOERROR                0x00000000
#define EXIT_COMMONERROR            0x00000001
#define EXIT_INVALID_FILE           0x00000002
#define EXIT_INVALID_GRAPHICFILTER  0x00000004
#define EXIT_INVALID_INPUTGRAPHIC   0x00000008
#define EXIT_OUTPUTERROR            0x00000010

// ----------
// - G2GApp -
// ----------

class G2GApp : public Application
{
private:

    String          aOutputFileName;
    BYTE            cExitCode;

    void            ShowUsage();
    BOOL            GetCommandOption( const String& rSwitch, String& rSwitchParam );
    void            SetExitCode( BYTE cExit )
                    {
                        if( ( EXIT_NOERROR == cExitCode ) || ( cExit != EXIT_NOERROR ) )
                            cExitCode = cExit;
                    }

    virtual void    Message( const String& rText, BYTE cExitCode = EXIT_NOERROR );

public:

    void            Main();
};

// -----------------------------------------------------------------------

BOOL G2GApp::GetCommandOption( const String& rSwitch, String& rFollowingParam )
{
    BOOL bRet = FALSE;

    for( USHORT i = 0, nCount = GetCommandLineParamCount(); ( i < nCount ) && !bRet; i++ )
    {
        String  aTestStr( '-' );

        for( long n = 0; ( n < 2 ) && !bRet; n++ )
        {
            aTestStr += rSwitch;

            if( aTestStr.CompareIgnoreCaseToAscii( GetCommandLineParam( i ) ) == COMPARE_EQUAL )
            {
                bRet = TRUE;

                if( i < ( nCount - 1 ) )
                    rFollowingParam = GetCommandLineParam( i + 1 );
                else
                    rFollowingParam = String();
            }

            if( 0 == n )
                aTestStr = '/';
        }
    }

    return bRet;
}

// -----------------------------------------------------------------------

void G2GApp::Message( const String& rText, BYTE cExitCode )
{
    if( EXIT_NOERROR != cExitCode )
        SetExitCode( cExitCode );

    ByteString aText( rText, RTL_TEXTENCODING_UTF8 );
#ifdef UNX
    aText.Append( "\r\n" );
#else
    aText.Append( "\n" );
#endif
    fprintf( stderr, aText.GetBuffer() );
}

// -----------------------------------------------------------------------------

void G2GApp::ShowUsage()
{
    Message( String() );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "usage:   g2g inputfile outputfile -f filter [ -# RRGGBB ]" ) ) );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "param:   inputfile: complete path to input image" ) ) );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "param:   outputfile: complete path to output image" ) ) );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "param:   -f: short name of filter to use ( e.g. gif, png org jpg )" ) ) );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "param:   -#: optional transparent color as hex value" ) ) );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "example: g2g /home/test.bmp /home/test.jpg -f jpg" ) ) );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "example: g2g /home/test.bmp /home/test.gif -f gif -# C0C0C0" ) ) );
}

// -----------------------------------------------------------------------------

void G2GApp::Main( )
{
    int     nCmdCount = GetCommandLineParamCount();
    USHORT  nCurCmd = 0;

    cExitCode = EXIT_NOERROR;

    if( !getenv( "STAR_RESOURCEPATH" ) )
        Message( String( RTL_CONSTASCII_USTRINGPARAM( "STAR_RESOURCEPATH environment variable not set" ) ), EXIT_INVALID_FILE );
    else if( nCmdCount >= 3 )
    {
        GraphicFilter   aFilter( sal_False );
        String          aInFile, aOutFile, aFilterStr, aTransColStr;

        aFilter.SetFilterPath( String( getenv( "STAR_RESOURCEPATH" ), RTL_TEXTENCODING_ASCII_US ) );

        aInFile = GetCommandLineParam( nCurCmd++ );
        aOutFile = GetCommandLineParam( nCurCmd++ );
        GetCommandOption( 'f', aFilterStr );
        GetCommandOption( '#', aTransColStr );

        if( aInFile.Len() && aOutFile.Len() && aFilterStr.Len() )
        {
            const USHORT nExportFilter = aFilter.GetExportFormatNumberForShortName( aFilterStr );

            if( GRFILTER_FORMAT_NOTFOUND == nExportFilter )
                Message( String( RTL_CONSTASCII_USTRINGPARAM( "invalid graphic filter" ) ), EXIT_INVALID_GRAPHICFILTER );
            else
            {
                if( DirEntry( aInFile ).Exists() )
                {
                    SvFileStream    aInStm( aInFile, STREAM_READ );
                    Graphic         aGraphic;
                    const GfxLink   aGfxLink;

                    aGraphic.SetLink( aGfxLink );

                    if( aFilter.ImportGraphic( aGraphic, aInFile, aInStm ) == GRFILTER_OK )
                    {
                        SvFileStream aOutStm( aOutFile, STREAM_WRITE | STREAM_TRUNC );

                        aFilter.ExportGraphic( aGraphic, aOutFile, aOutStm, nExportFilter );

                        if( aOutStm.GetError() )
                            Message( String( RTL_CONSTASCII_USTRINGPARAM( "could not write output file" ) ), EXIT_OUTPUTERROR );
                    }
                    else
                        Message( String( RTL_CONSTASCII_USTRINGPARAM( "could import graphic" ) ), EXIT_INVALID_INPUTGRAPHIC );
                }
                else
                    Message( String( RTL_CONSTASCII_USTRINGPARAM( "invalid file(s)" ) ), EXIT_INVALID_FILE );
            }
        }
    }
    else
    {
        ShowUsage();
        Message( String(), EXIT_COMMONERROR );
    }

    if( EXIT_NOERROR != cExitCode )
        raise( SIGABRT );
}

// ---------------
// - Application -
// ---------------

G2GApp aApp;
