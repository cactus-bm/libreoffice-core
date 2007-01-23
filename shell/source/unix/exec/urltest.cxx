/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: urltest.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2007-01-23 12:28:11 $
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

#include "shellexec.hxx"

#include <osl/process.h>

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <strings.h>

// -----------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    int ret = 0;

    if( argc != 2 )
    {
        fprintf(stderr, "Usage: urltest <urllist>\n");
        return -1;
    }

    FILE * fp = fopen( argv[1], "r" );
    if( NULL == fp )
    {
        perror( argv[1] );
        return -1;
    }

    // expect urltest.sh beside this binary
    char line[LINE_MAX];
    size_t len = strlen(argv[0]);
    strcpy( line, argv[0] );
    strcpy( line + len, ".sh " );
    len += 4;

    unsigned int errors = 0;

    // read url(s) to test from file
    char url[512];
    while( NULL != fgets(url, sizeof(url), fp))
    {
        // remove trailing line break
        strtok( url, "\r\n" );

        printf( "Passing URL: %s\n", url );

        // test the encoding functionality from shellexec.cxx
        rtl::OString aURL( url );
        rtl::OStringBuffer aBuffer;
        escapeForShell(aBuffer, aURL);

        // append encoded URL as (only) parameter to the script
        strcpy( line + len, aBuffer.getStr() );

        printf( "Command line: %s\n", line );

        FILE * pipe = popen( line, "r" );
        if( NULL != pipe )
        {
            char buffer[BUFSIZ];

            // initialize buffer with '\0'
            memset(buffer, '\0', BUFSIZ);

            // read the output of the script
            if(NULL == fgets( buffer, BUFSIZ, pipe))
            {
                perror("FAILED: output of script could not be read");
                printf( "\n");
                ++errors;
                continue;
            }

            // remove trailing line break again
            strtok( buffer, "\r\n" );

            int n = pclose(pipe);
            if( 0 != n )
            {
                printf("FAILED: fclose returned %d\n\n", n );
                ++errors;
                continue;
            }

            if( 0 == strcmp( url, buffer ) )
            {
                // strings are identical: good !
                printf( "OK\n\n");
            }
            else
            {
                // compare failed
                printf( "FAILED: returned string is %s\n\n", buffer);
                ++errors;
            }

        }
        else
        {
            perror( line );
            ret = -2;
            break;
        }
    }

    if( ferror( fp ) )
    {
        perror( argv[1] );
        ret = -1;
    }

    fclose( fp );

    if( errors )
    {
        printf( "Number of tests failing: %d\n", errors);
        ret = -3;
    }
    else
        printf( "All tests passed OK.\n" );


    return ret;
}
