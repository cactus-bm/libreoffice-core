/*************************************************************************
 *
 *  $RCSfile: bmp.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: ka $ $Date: 2002-01-10 10:08:41 $
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
#include <vector>
#include <vcl/svapp.hxx>

#include "solar.hrc"
#include "filedlg.hxx"
#include "bmpcore.hxx"
#include "bmp.hrc"

// ----------
// - BmpApp -
// ----------

class BmpApp : public BmpCreator
{
private:

    String          aOutputFileName;
    BYTE            cExitCode;

    BOOL            GetCommandOption( const ::std::vector< String >& rArgs, const String& rSwitch, String& rSwitchParam );
    void            SetExitCode( BYTE cExit )
                    {
                        if( ( EXIT_NOERROR == cExitCode ) || ( cExit != EXIT_NOERROR ) )
                            cExitCode = cExit;
                    }
    void            ShowUsage();

    virtual void    Message( const String& rText, BYTE cExitCode );

public:

                    BmpApp();
                    ~BmpApp();

    int             Start( const ::std::vector< String >& rArgs );
};

// -----------------------------------------------------------------------------

BmpApp::BmpApp()
{
}

// -----------------------------------------------------------------------------

BmpApp::~BmpApp()
{
}

// -----------------------------------------------------------------------

BOOL BmpApp::GetCommandOption( const ::std::vector< String >& rArgs, const String& rSwitch, String& rFollowingParam )
{
    BOOL bRet = FALSE;

    for( int i = 0, nCount = rArgs.size(); ( i < nCount ) && !bRet; i++ )
    {
        String  aTestStr( '-' );

        for( int n = 0; ( n < 2 ) && !bRet; n++ )
        {
            aTestStr += rSwitch;

            if( aTestStr.CompareIgnoreCaseToAscii( rArgs[ i ] ) == COMPARE_EQUAL )
            {
                bRet = TRUE;

                if( i < ( nCount - 1 ) )
                    rFollowingParam = rArgs[ i + 1 ];
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

void BmpApp::Message( const String& rText, BYTE cExitCode )
{
    if( EXIT_NOERROR != cExitCode )
        SetExitCode( cExitCode );

    ByteString aText( rText, RTL_TEXTENCODING_UTF8 );
    aText.Append( "\r\n" );
    fprintf( stderr, aText.GetBuffer() );
}

// -----------------------------------------------------------------------------

void BmpApp::ShowUsage()
{
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "Usage:" ) ), EXIT_NOERROR );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "    bmp srs_inputfile bmp_dir output_dir lang_dir [-f err_file]" ) ), EXIT_NOERROR );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "Options:" ) ), EXIT_NOERROR );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "   -f            name of file output should be written to" ) ), EXIT_NOERROR );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "Examples:" ) ), EXIT_NOERROR );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "    bmp /home/test.srs /home/res /home/out /home/res/enus" ) ), EXIT_NOERROR );
}

// -----------------------------------------------------------------------------

int BmpApp::Start( const ::std::vector< String >& rArgs )
{
    String aOutName;

    cExitCode = EXIT_NOERROR;

    if( rArgs.size() >= 4 )
    {
        LangInfo            aLangInfo;
        USHORT              nCurCmd = 0;
        const String        aSrsName( rArgs[ nCurCmd++ ] );
        const String        aInName( rArgs[ nCurCmd++ ] );
        ByteString          aLangDir;

        aOutName = rArgs[ nCurCmd++ ];
        aLangDir = ByteString( rArgs[ nCurCmd++ ], RTL_TEXTENCODING_ASCII_US );

        GetCommandOption( rArgs, 'f', aOutputFileName );

        memcpy( aLangInfo.maLangDir, aLangDir.GetBuffer(), aLangDir.Len() + 1 );
        aLangInfo.mnLangNum = (USHORT) DirEntry( aOutName ).GetName().ToInt32();

        Create( aSrsName, aInName, aOutName, aLangInfo );
    }
    else
    {
        ShowUsage();
        cExitCode = EXIT_COMMONERROR;
    }

    if( ( EXIT_NOERROR == cExitCode ) && aOutputFileName.Len() && aOutName.Len() )
    {
        SvFileStream    aOStm( aOutputFileName, STREAM_WRITE | STREAM_TRUNC );
        ByteString      aStr( "Successfully generated ImageList(s) in: " );

        aOStm.WriteLine( aStr.Append( ByteString( aOutName, RTL_TEXTENCODING_UTF8 ) ) );
        aOStm.Close();
    }

    if ( EXIT_NOERROR != cExitCode )
        raise( SIGABRT );

    return cExitCode;
}

// --------
// - Main -
// --------

int main( int nArgCount, char* ppArgs[] )
{
    ::std::vector< String > aArgs;
    BmpApp                  aBmpApp;

    for( int i = 1; i < nArgCount; i++ )
        aArgs.push_back( String( ppArgs[ i ], RTL_TEXTENCODING_ASCII_US ) );

    return aBmpApp.Start( aArgs );
}

#if 0

// ---------------
// - Application -
// ---------------

class ApplicationWrapper : public Application
{
protected:

    virtual void    Main();

public:

                    ApplicationWrapper();
                    ~ApplicationWrapper();
};

// -----------------------------------------------------------------------------

ApplicationWrapper::ApplicationWrapper()
{
}

// -----------------------------------------------------------------------------

ApplicationWrapper::~ApplicationWrapper()
{
}

// -----------------------------------------------------------------------------

void ApplicationWrapper::Main()
{
    ::std::vector< String > aArgs;
    BmpApp                  aBmpApp;

    for( int i = 0; i < GetCommandLineParamCount(); i++ )
        aArgs.push_back( GetCommandLineParam( i ) );

    aBmpApp.Start( aArgs );
}

// -----------------------------------------------------------------------------

ApplicationWrapper aApp;

#endif
