/*************************************************************************
 *
 *  $RCSfile: cdeint.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: pl $ $Date: 2002-03-01 12:56:44 $
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
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <salunx.h>
#include <saldisp.hxx>
#include <cdeint.hxx>
#include <salbmp.hxx>
#include <salframe.hxx>
#include <soicon.hxx>
#include <tools/stream.hxx>
#include <tools/debug.hxx>

CDEIntegrator::CDEIntegrator( SalFrame* pFrame ) :
        DtIntegrator( pFrame )
{
    meType = DtCDE;
    mnRefCount++;
}

CDEIntegrator::~CDEIntegrator()
{
    mnRefCount--;
}

static int getHexDigit( const char c )
{
    if( c >= '0' && c <= '9' )
        return (int)(c-'0');
    else if( c >= 'a' && c <= 'f' )
        return (int)(c-'a'+10);
    else if( c >= 'A' && c <= 'F' )
        return (int)(c-'A'+10);
    return -1;
}


BOOL CDEIntegrator::GetSystemLook( SystemLookInfo& rInfo )
{
    static Color aColors[ 8 ];
    static sal_Bool bRead = sal_False;

    if( ! bRead )
    {
        // get used palette from xrdb
        char **ppStringList = 0;
        int nStringCount;
        XTextProperty aTextProperty;
        aTextProperty.value = 0;
        int i;

        static Atom nResMgrAtom = XInternAtom( mpDisplay, "RESOURCE_MANAGER", False );

        if( XGetTextProperty( mpDisplay,
                              RootWindow( mpDisplay, 0 ),
                              &aTextProperty,
                              nResMgrAtom )
            && aTextProperty.value
            && XTextPropertyToStringList( &aTextProperty, &ppStringList, &nStringCount )
            )
        {
            // format of ColorPalette resource:
            // *n*ColorPalette: palettefile

            ByteString aLines;
            for( i=0; i < nStringCount; i++ )
                aLines += ppStringList[i];
            for( i = aLines.GetTokenCount( '\n' )-1; i >= 0; i-- )
            {
                ByteString aLine = aLines.GetToken( i, '\n' );
                int nIndex = aLine.Search( "ColorPalette" );
                if( nIndex != STRING_NOTFOUND )
                {
                    int nPos = nIndex;

                    nIndex+=12;
                    const char* pStr = aLine.GetBuffer() +nIndex;
                    while( *pStr && isspace( *pStr ) && *pStr != ':' )
                    {
                        pStr++;
                        nIndex++;
                    }
                    if( *pStr != ':' )
                        continue;
                    pStr++, nIndex++;
                    for( ; *pStr && isspace( *pStr ); pStr++, nIndex++ )
                        ;
                    if( ! *pStr )
                        continue;
                    int nIndex2 = nIndex;
                    for( ; *pStr && ! isspace( *pStr ); pStr++, nIndex2++ )
                        ;
                    ByteString aPaletteFile( aLine.Copy( nIndex, nIndex2 - nIndex ) );
                    // extract number before ColorPalette;
                    for( ; nPos >= 0 && aLine.GetChar( nPos ) != '*'; nPos-- )
                        ;
                    nPos--;
                    for( ; nPos >= 0 && aLine.GetChar( nPos ) != '*'; nPos-- )
                        ;
                    int nNumber = aLine.Copy( ++nPos ).ToInt32();

                    DBG_TRACE2( "found palette %d in resource \"%s\"", nNumber, aLine.GetBuffer() );

                    // found no documentation what this number actually means;
                    // might be the screen number. 0 seems to be the right one
                    // in most cases.
                    if( nNumber )
                        continue;

                    DBG_TRACE1( "Palette file is \"%s\".\n", aPaletteFile.GetBuffer() );

                    String aPath( aHomeDir );
                    aPath.AppendAscii( "/.dt/palettes/" );
                    aPath += String( aPaletteFile, gsl_getSystemTextEncoding() );

                    SvFileStream aStream( aPath, STREAM_READ );
                    if( ! aStream.IsOpen() )
                    {
                        aPath = String::CreateFromAscii( "/usr/dt/palettes/" );
                        aPath += String( aPaletteFile, gsl_getSystemTextEncoding() );
                        aStream.Open( aPath, STREAM_READ );
                        if( ! aStream.IsOpen() )
                            continue;
                    }

                    ByteString aBuffer;
                    for( nIndex = 0; nIndex < 8; nIndex++ )
                    {
                        aStream.ReadLine( aBuffer );
                        // format is "#RRRRGGGGBBBB"

                        DBG_TRACE1( "\t\"%s\".\n", aBuffer.GetBuffer() );

                        if( aBuffer.Len() )
                        {
                            const char* pArr = (const char*)aBuffer.GetBuffer()+1;
                            aColors[nIndex] = Color(
                                getHexDigit( pArr[1] )
                                | ( getHexDigit( pArr[0] ) << 4 ),
                                getHexDigit( pArr[5] )
                                | ( getHexDigit( pArr[4] ) << 4 ),
                                getHexDigit( pArr[9] )
                                | ( getHexDigit( pArr[8] ) << 4 )
                                );

                            DBG_TRACE1( "\t\t%lx\n", aColors[nIndex].GetColor() );
                        }
                    }

                    break;
                }
            }
        }

        if( ppStringList )
            XFreeStringList( ppStringList );
        if( aTextProperty.value )
            XFree( aTextProperty.value );
    }

    rInfo.windowActiveStart = aColors[0];
    rInfo.windowActiveEnd = aColors[0];
    rInfo.activeBorder = aColors[0];

    rInfo.windowInactiveStart = aColors[1];
    rInfo.windowInactiveEnd = aColors[1];
    rInfo.inactiveBorder = aColors[1];

    rInfo.activeForeground =
        aColors[ 0 ].GetBlue() < 128        ||
        aColors[ 0 ].GetGreen() < 128       ||
        aColors[ 0 ].GetRed() < 128
        ? Color( COL_WHITE ) : Color( COL_BLACK );
    rInfo.inactiveForeground =
        aColors[ 1 ].GetBlue() < 128        ||
        aColors[ 1 ].GetGreen() < 128       ||
        aColors[ 1 ].GetRed() < 128
        ? Color( COL_WHITE ) : Color( COL_BLACK );
    rInfo.foreground    = rInfo.inactiveForeground;
    rInfo.background    = aColors[ 1 ];

    return TRUE;
}
