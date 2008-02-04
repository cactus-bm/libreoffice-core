/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: helper.cxx,v $
 *
 *  $Revision: 1.30 $
 *
 *  last change: $Author: ihi $ $Date: 2008-02-04 13:35:18 $
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
#include "precompiled_psprint.hxx"

#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#include <psprint/helper.hxx>
#include <tools/string.hxx>
#include <tools/urlobj.hxx>
#include <osl/file.hxx>
#include <osl/process.h>
#include <osl/thread.h>
#include <tools/config.hxx>
#include <rtl/bootstrap.hxx>
#include <sal/config.h>
#ifdef SOLAR_JAVA
#include <jvmfwk/framework.h>
#endif // SOLAR_JAVA

#ifdef MACOSX
// directories of OS X fonts
#define MACXP_FONT_DIR        "/Library/Fonts;/System/Library/Fonts;"
#endif

using namespace rtl;

namespace psp {

const OUString& getOfficePath( enum whichOfficePath ePath )
{
    static OUString aNetPath;
    static OUString aUserPath;
    static OUString aConfigPath;
    static OUString aEmpty;
    static bool bOnce = false;

    if( ! bOnce )
    {
        bOnce = true;
        OUString aIni;
        osl_getExecutableFile( &aIni.pData );
        aIni = aIni.copy( 0, aIni.lastIndexOf( '/' )+1 );
        aIni += OUString( RTL_CONSTASCII_USTRINGPARAM( SAL_CONFIGFILE( "bootstrap" ) ) );
        Bootstrap aBootstrap( aIni );
        aBootstrap.getFrom( OUString( RTL_CONSTASCII_USTRINGPARAM( "CustomDataUrl" ) ), aConfigPath );
        aBootstrap.getFrom( OUString( RTL_CONSTASCII_USTRINGPARAM( "BaseInstallation" ) ), aNetPath );
        aBootstrap.getFrom( OUString( RTL_CONSTASCII_USTRINGPARAM( "UserInstallation" ) ), aUserPath );
        OUString aUPath = aUserPath;

        if( ! aConfigPath.compareToAscii( "file://", 7 ) )
        {
            OUString aSysPath;
            if( osl_getSystemPathFromFileURL( aConfigPath.pData, &aSysPath.pData ) == osl_File_E_None )
                aConfigPath = aSysPath;
        }
        if( ! aNetPath.compareToAscii( "file://", 7 ) )
        {
            OUString aSysPath;
            if( osl_getSystemPathFromFileURL( aNetPath.pData, &aSysPath.pData ) == osl_File_E_None )
                aNetPath = aSysPath;
        }
        if( ! aUserPath.compareToAscii( "file://", 7 ) )
        {
            OUString aSysPath;
            if( osl_getSystemPathFromFileURL( aUserPath.pData, &aSysPath.pData ) == osl_File_E_None )
                aUserPath = aSysPath;
        }
        // ensure user path exists
        aUPath += OUString( RTL_CONSTASCII_USTRINGPARAM( "/user/psprint" ) );
        #if OSL_DEBUG_LEVEL > 1
        oslFileError eErr =
        #endif
        osl_createDirectoryPath( aUPath.pData, NULL, NULL );
        #if OSL_DEBUG_LEVEL > 1
        fprintf( stderr, "try to create \"%s\" = %d\n", OUStringToOString( aUPath, RTL_TEXTENCODING_UTF8 ).getStr(), eErr );
        #endif
    }

    switch( ePath )
    {
        case ConfigPath: return aConfigPath;
        case NetPath: return aNetPath;
        case UserPath: return aUserPath;
    }
    return aEmpty;
}

static OString getEnvironmentPath( const char* pKey )
{
    OString aPath;

    const char* pValue = getenv( pKey );
    if( pValue && *pValue )
    {
        aPath = OString( pValue );
    }
    return aPath;
}

} // namespace psp

void psp::getPrinterPathList( std::list< OUString >& rPathList, const char* pSubDir )
{
    rPathList.clear();
    rtl_TextEncoding aEncoding = osl_getThreadTextEncoding();

    OUStringBuffer aPathBuffer( 256 );

    // append net path
    aPathBuffer.append( getOfficePath( psp::NetPath ) );
    if( aPathBuffer.getLength() )
    {
        aPathBuffer.appendAscii( "/share/psprint" );
        if( pSubDir )
        {
            aPathBuffer.append( sal_Unicode('/') );
            aPathBuffer.appendAscii( pSubDir );
        }
        rPathList.push_back( aPathBuffer.makeStringAndClear() );
    }
    // append user path
    aPathBuffer.append( getOfficePath( psp::UserPath ) );
    if( aPathBuffer.getLength() )
    {
        aPathBuffer.appendAscii( "/user/psprint" );
        if( pSubDir )
        {
            aPathBuffer.append( sal_Unicode('/') );
            aPathBuffer.appendAscii( pSubDir );
        }
        rPathList.push_back( aPathBuffer.makeStringAndClear() );
    }

    OString aPath( getEnvironmentPath("SAL_PSPRINT") );
    sal_Int32 nIndex = 0;
    do
    {
        OString aDir( aPath.getToken( 0, ':', nIndex ) );
        if( ! aDir.getLength() )
            continue;

        if( pSubDir )
        {
            aDir += "/";
            aDir += pSubDir;
        }
        struct stat aStat;
        if( stat( aDir.getStr(), &aStat ) || ! S_ISDIR( aStat.st_mode ) )
            continue;

        rPathList.push_back( OStringToOUString( aDir, aEncoding ) );
    } while( nIndex != -1 );

    if( rPathList.empty() )
    {
        // last resort: next to program file (mainly for setup)
        OUString aExe;
        if( osl_getExecutableFile( &aExe.pData ) == osl_Process_E_None )
        {
            INetURLObject aDir( aExe );
            aDir.removeSegment();
            aExe = aDir.GetMainURL( INetURLObject::NO_DECODE );
            OUString aSysPath;
            if( osl_getSystemPathFromFileURL( aExe.pData, &aSysPath.pData ) == osl_File_E_None )
            {
                rPathList.push_back( aSysPath );
            }
        }
    }
}

const OUString& psp::getFontPath()
{
    static OUString aPath;

    if( ! aPath.getLength() )
    {
        OUStringBuffer aPathBuffer( 512 );

        OUString aConfigPath = getOfficePath( psp::ConfigPath );
        OUString aNetPath = getOfficePath( psp::NetPath );
        OUString aUserPath = getOfficePath( psp::UserPath );
        if( aConfigPath.getLength() )
        {
            // #i53530# Path from CustomDataUrl will completely
            // replace net and user paths if the path exists
            aPathBuffer.append(aConfigPath);
            aPathBuffer.appendAscii("/share/fonts");
            // check existance of config path
            struct stat aStat;
            if( 0 != stat( OUStringToOString( aPathBuffer.makeStringAndClear(), osl_getThreadTextEncoding() ).getStr(), &aStat )
                || ! S_ISDIR( aStat.st_mode ) )
                aConfigPath = OUString();
            else
            {
                aPathBuffer.append(aConfigPath);
                aPathBuffer.appendAscii("/share/fonts");
            }
        }
        if( aConfigPath.getLength() == 0 )
        {
            if( aNetPath.getLength() )
            {
                aPathBuffer.append( aNetPath );
                aPathBuffer.appendAscii( "/share/fonts/truetype;");
                aPathBuffer.append( aNetPath );
                aPathBuffer.appendAscii( "/share/fonts/type1;" );
            }
            if( aUserPath.getLength() )
            {
                aPathBuffer.append( aUserPath );
                aPathBuffer.appendAscii( "/user/fonts" );
            }
        }
        OString aEnvPath( getEnvironmentPath( "SAL_FONTPATH_PRIVATE" ) );
        if( aEnvPath.getLength() )
        {
            aPathBuffer.append( sal_Unicode(';') );
            aPathBuffer.append( OStringToOUString( aEnvPath, osl_getThreadTextEncoding() ) );
        }

#ifdef MACOSX   // Search for truetype fonts also in the MACOSX system paths

    aPathBuffer.append(sal_Unicode(';') );
    aPath += OUString( RTL_CONSTASCII_USTRINGPARAM(MACXP_FONT_DIR) );

    //Userfonts
    aPath += aUserPath;
    // #i67231# [ericb 07/06] aPath was not concatened with other paths
    if (aUserPath.getLength() )
    {
        aPathBuffer.append(aPath);
        aPathBuffer.appendAscii("/../../Fonts");
    }
#endif // MACOSX

        // append jre/jdk fonts if possible
        OString aJREpath;

#ifdef SOLAR_JAVA
        JavaInfo * pInfo = NULL;
        javaFrameworkError jerr = jfw_getSelectedJRE( & pInfo);
        if (jerr == JFW_E_NONE && pInfo != NULL)
        {
            OUString aSys;
            if( osl_getSystemPathFromFileURL( pInfo->sLocation, &aSys.pData ) == osl_File_E_None )
                aJREpath = OUStringToOString( aSys, osl_getThreadTextEncoding() );
        }
        jfw_freeJavaInfo(pInfo);
#endif

        if( aJREpath.getLength() > 0 )
        {
            OString aTestPath( aJREpath );
            aTestPath += "/jre/lib/fonts";
            if( access( aTestPath.getStr(), R_OK ) )
            {
                aTestPath = aJREpath;
                aTestPath += "/lib/fonts";
                if( access( aTestPath.getStr(), R_OK ) )
                    aJREpath = OString();
                else
                    aJREpath = aTestPath;
            }
            else
                aJREpath = aTestPath;
        }

        if( aJREpath.getLength() )
        {
            aPathBuffer.append( sal_Unicode(';') );
            aPathBuffer.append( OStringToOUString( aJREpath, osl_getThreadTextEncoding() ) );
        }

        aPath = aPathBuffer.makeStringAndClear();
#if OSL_DEBUG_LEVEL > 1
        fprintf( stderr, "initializing font path to \"%s\"\n", OUStringToOString( aPath, RTL_TEXTENCODING_ISO_8859_1 ).getStr() );
#endif
    }
    return aPath;
}

bool psp::convertPfbToPfa( ::osl::File& rInFile, ::osl::File& rOutFile )
{
    static unsigned char hexDigits[] =
        {
            '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
        };

    bool bSuccess = true;
    bool bEof = false;
    unsigned char buffer[256];
    sal_uInt64 nRead;
    sal_uInt64 nOrgPos = 0;
    rInFile.getPos( nOrgPos );

    while( bSuccess && ! bEof )
    {
        // read leading bytes
        bEof = ! rInFile.read( buffer, 6, nRead ) && nRead == 6 ? false : true;
        unsigned int nType = buffer[ 1 ];
        unsigned int nBytesToRead = buffer[2] | buffer[3] << 8 | buffer[4] << 16 | buffer[5] << 24;
        if( buffer[0] != 0x80 ) // test for pfb m_agic number
        {
            // this migt be a pfa font already
            sal_uInt64 nWrite = 0;
            if( ! rInFile.read( buffer+6, 9, nRead ) && nRead == 9 &&
                ( ! strncmp( (char*)buffer, "%!FontType1-", 12 ) ||
                  ! strncmp( (char*)buffer, "%!PS-AdobeFont-", 15 ) ) )
            {
                if( rOutFile.write( buffer, 15, nWrite ) || nWrite != 15 )
                    bSuccess = false;
                while( bSuccess &&
                       ! rInFile.read( buffer, sizeof( buffer ), nRead ) &&
                       nRead != 0 )
                {
                    if( rOutFile.write( buffer, nRead, nWrite ) ||
                        nWrite != nRead )
                        bSuccess = false;
                }
                bEof = true;
            }
            else
                bSuccess = false;
        }
        else if( nType == 1 || nType == 2 )
        {
            unsigned char* pBuffer = new unsigned char[ nBytesToRead+1 ];

            if( ! rInFile.read( pBuffer, nBytesToRead, nRead ) && nRead == nBytesToRead )
            {
                if( nType == 1 )
                {
                    // ascii data, convert dos lineends( \r\n ) and
                    // m_ac lineends( \r ) to \n
                    unsigned char * pWriteBuffer = new unsigned char[ nBytesToRead ];
                    unsigned int nBytesToWrite = 0;
                    for( unsigned int i = 0; i < nBytesToRead; i++ )
                    {
                        if( pBuffer[i] != '\r' )
                            pWriteBuffer[ nBytesToWrite++ ] = pBuffer[i];
                        else if( pBuffer[ i+1 ] == '\n' )
                        {
                            i++;
                            pWriteBuffer[ nBytesToWrite++ ] = '\n';
                        }
                        else
                            pWriteBuffer[ nBytesToWrite++ ] = '\n';
                    }
                    if( rOutFile.write( pWriteBuffer, nBytesToWrite, nRead ) || nRead != nBytesToWrite )
                        bSuccess = false;

                    delete [] pWriteBuffer;
                }
                else
                {
                    // binary data
                    unsigned int nBuffer = 0;
                    for( unsigned int i = 0; i < nBytesToRead && bSuccess; i++ )
                    {
                        buffer[ nBuffer++ ] = hexDigits[ pBuffer[ i ] >> 4 ];
                        buffer[ nBuffer++ ] = hexDigits[ pBuffer[ i ] & 15 ];
                        if( nBuffer >= 80 )
                        {
                            buffer[ nBuffer++ ] = '\n';
                            if( rOutFile.write( buffer, nBuffer, nRead ) || nRead != nBuffer )
                                bSuccess = false;
                            nBuffer = 0;
                        }
                    }
                    if( nBuffer > 0 && bSuccess )
                    {
                        buffer[ nBuffer++ ] = '\n';
                        if( rOutFile.write( buffer, nBuffer, nRead ) || nRead != nBuffer )
                            bSuccess = false;
                    }
                }
            }
            else
                bSuccess = false;

            delete [] pBuffer;
        }
        else if( nType == 3 )
            bEof = true;
        else
            bSuccess = false;
    }

    return bSuccess;
}

void psp::normPath( OString& rPath )
{
    char buf[PATH_MAX];

    ByteString aPath( rPath );

    // double slashes and slash at end are probably
    // removed by realpath anyway, but since this runs
    // on many different platforms let's play it safe
    while( aPath.SearchAndReplace( "//", "/" ) != STRING_NOTFOUND )
        ;
    if( aPath.Len() > 0 && aPath.GetChar( aPath.Len()-1 ) == '/' )
        aPath.Erase( aPath.Len()-1 );

    if( ( aPath.Search( "./" ) != STRING_NOTFOUND ||
          aPath.Search( "~" ) != STRING_NOTFOUND )
        && realpath( aPath.GetBuffer(), buf ) )
    {
        rPath = buf;
    }
    else
    {
        rPath = aPath;
    }
}

void psp::splitPath( OString& rPath, OString& rDir, OString& rBase )
{
    normPath( rPath );
    sal_Int32 nIndex = rPath.lastIndexOf( '/' );
    if( nIndex > 0 )
        rDir = rPath.copy( 0, nIndex );
    else if( nIndex == 0 ) // root dir
        rDir = rPath.copy( 0, 1 );
    if( rPath.getLength() > nIndex+1 )
        rBase = rPath.copy( nIndex+1 );
}


