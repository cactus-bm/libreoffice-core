/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: res.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 09:42:57 $
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

#if OSL_DEBUG_LEVEL == 0
#  ifndef NDEBUG
#    define NDEBUG
#  endif
#endif
#include <assert.h>

#include <interface.hxx>
#include <cstdio>
#include <hash_map>
#include <string>

using namespace std;

static hash_map< string, string >* pStringResources = NULL;

static string getResFileName( const char* progname )
{
    string aRet = progname;
    size_t pos = aRet.rfind( '/' );
    // FIXME: search PATH if necessary
    assert( pos != string::npos );
    aRet.erase( pos );
    aRet.append( "/resource/crash_dump.res" );

    return aRet;
}

static void filterString( string& rString )
{
    static const char* pProductName = getenv( "PRODUCTNAME" );
    static int nProductLen = pProductName ? strlen( pProductName ) : 0;
    static const char* pProductVersion = getenv( "PRODUCTVERSION" );
    static int nVersionLen = pProductVersion ? strlen( pProductVersion ) : 0;

    // fill in eventually escaped characters
    string::size_type pos = 0;
    while( (pos = rString.find( '\\' ) ) != string::npos )
    {
        char cRep = 0;
        switch( rString[pos+1] )
        {
            case 't': cRep = '\t';break;
            case 'n': cRep = '\n';break;
            case 'r': cRep = '\r';break;
            case 'f': cRep = '\f';break;
            default: cRep = rString[pos+1];
        }
        if( cRep )
            rString.replace( pos, 2, &cRep, 1 );
    }
    while( (pos = rString.find( '~' ) ) != string::npos )
    {
        // replace mnemonic marker
        rString.replace( pos, 1, "_", 1 );
    }
    while( (pos = rString.find( "%PRODUCTNAME%" ) ) != string::npos )
    {
        rString.replace( pos, 13, pProductName ?  pProductName : "OpenOffice" );
    }
    while( (pos = rString.find( "%PRODUCTVERSION%" ) ) != string::npos )
    {
        rString.replace( pos, 16, pProductVersion ? pProductVersion : "" );
    }
    // remove whitespace at end
    pos = rString.find_last_not_of( "\r\n\t\f " );
    if( pos != string::npos )
          rString.erase( pos+1 );
}

void StringResource::init( int argc, char** argv )
{
    pStringResources = new hash_map< string, string >();

    string aResFile = getResFileName( argv[0] );

    FILE* fp = fopen( aResFile.c_str(), "r" );
    if( fp )
    {
        char buf[4096];
        string aKey;
        string aValue;
        while( ! feof( fp ) )
        {
            if( ! fgets( buf, sizeof(buf), fp ) )
                break;

            char* pEq = strchr( buf, '=' );
            if( ! pEq || *(pEq+1) == 0 ) // invalid line
                continue;
            aKey = string(buf, pEq-buf);
            aValue = pEq+1;
            while( (aValue.empty() || aValue[ aValue.size()-1 ] != '\n') && ! feof( fp ) )
            {
                if( fgets( buf, sizeof( buf ), fp ) )
                    aValue.append( buf );
            }
            filterString( aValue );
            (*pStringResources)[aKey] = aValue;
        }
        fclose( fp );
    }
}

const char* StringResource::get( const char* pKey )
{
    hash_map< string, string >::const_iterator it = pStringResources->find( pKey );
    return (it == pStringResources->end()) ? "" : it->second.c_str();
}

