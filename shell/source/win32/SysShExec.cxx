/*************************************************************************
 *
 *  $RCSfile: SysShExec.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: tra $ $Date: 2001-04-27 13:54:12 $
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

//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef _SYSSHEXEC_HXX_
#include "SysShExec.hxx"
#endif

#include <osl/file.h>

#ifndef _COM_SUN_STAR_SYS_SHELL_SYSTEMSHELLEXECUTEFLAGS_HPP_
#include <com/sun/star/system/SystemShellExecuteFlags.hpp>
#endif

#include <systools/win32/shell9x.h>

//------------------------------------------------------------------------
// namespace directives
//------------------------------------------------------------------------

using com::sun::star::uno::Reference;
using com::sun::star::uno::RuntimeException;
using com::sun::star::uno::Sequence;
using com::sun::star::uno::XInterface;
using com::sun::star::lang::EventObject;
using com::sun::star::lang::XServiceInfo;
using com::sun::star::lang::IllegalArgumentException;
using rtl::OUString;
using osl::Mutex;
using com::sun::star::system::XSystemShellExecute;
using com::sun::star::system::SystemShellExecuteException;

using namespace ::com::sun::star::system::SystemShellExecuteFlags;
using namespace cppu;

//------------------------------------------------------------------------
// defines
//------------------------------------------------------------------------

#define SYSSHEXEC_IMPL_NAME  "com.sun.star.sys.shell.SystemShellExecute"

//------------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------------

namespace // private
{
    Sequence< OUString > SAL_CALL SysShExec_getSupportedServiceNames()
    {
        Sequence< OUString > aRet(1);
        aRet[0] = OUString::createFromAscii("com.sun.star.sys.shell.SystemShellExecute");
        return aRet;
    }

    /* This is the error table that defines the mapping between OS error
    codes and errno values */

    struct errentry {
        unsigned long oscode;   /* OS return value */
        int errnocode;          /* System V error code */
    };

    struct errentry errtable[] = {
        {  ERROR_SUCCESS,                osl_File_E_None     },  /* 0 */
        {  ERROR_INVALID_FUNCTION,       osl_File_E_INVAL    },  /* 1 */
        {  ERROR_FILE_NOT_FOUND,         osl_File_E_NOENT    },  /* 2 */
        {  ERROR_PATH_NOT_FOUND,         osl_File_E_NOENT    },  /* 3 */
        {  ERROR_TOO_MANY_OPEN_FILES,    osl_File_E_MFILE    },  /* 4 */
        {  ERROR_ACCESS_DENIED,          osl_File_E_ACCES    },  /* 5 */
        {  ERROR_INVALID_HANDLE,         osl_File_E_BADF     },  /* 6 */
        {  ERROR_ARENA_TRASHED,          osl_File_E_NOMEM    },  /* 7 */
        {  ERROR_NOT_ENOUGH_MEMORY,      osl_File_E_NOMEM    },  /* 8 */
        {  ERROR_INVALID_BLOCK,          osl_File_E_NOMEM    },  /* 9 */
        {  ERROR_BAD_ENVIRONMENT,        osl_File_E_2BIG     },  /* 10 */
        {  ERROR_BAD_FORMAT,             osl_File_E_NOEXEC   },  /* 11 */
        {  ERROR_INVALID_ACCESS,         osl_File_E_INVAL    },  /* 12 */
        {  ERROR_INVALID_DATA,           osl_File_E_INVAL    },  /* 13 */
        {  ERROR_INVALID_DRIVE,          osl_File_E_NOENT    },  /* 15 */
        {  ERROR_CURRENT_DIRECTORY,      osl_File_E_ACCES    },  /* 16 */
        {  ERROR_NOT_SAME_DEVICE,        osl_File_E_XDEV     },  /* 17 */
        {  ERROR_NO_MORE_FILES,          osl_File_E_NOENT    },  /* 18 */
        {  ERROR_LOCK_VIOLATION,         osl_File_E_ACCES    },  /* 33 */
        {  ERROR_BAD_NETPATH,            osl_File_E_NOENT    },  /* 53 */
        {  ERROR_NETWORK_ACCESS_DENIED,  osl_File_E_ACCES    },  /* 65 */
        {  ERROR_BAD_NET_NAME,           osl_File_E_NOENT    },  /* 67 */
        {  ERROR_FILE_EXISTS,            osl_File_E_EXIST    },  /* 80 */
        {  ERROR_CANNOT_MAKE,            osl_File_E_ACCES    },  /* 82 */
        {  ERROR_FAIL_I24,               osl_File_E_ACCES    },  /* 83 */
        {  ERROR_INVALID_PARAMETER,      osl_File_E_INVAL    },  /* 87 */
        {  ERROR_NO_PROC_SLOTS,          osl_File_E_AGAIN    },  /* 89 */
        {  ERROR_DRIVE_LOCKED,           osl_File_E_ACCES    },  /* 108 */
        {  ERROR_BROKEN_PIPE,            osl_File_E_PIPE     },  /* 109 */
        {  ERROR_DISK_FULL,              osl_File_E_NOSPC    },  /* 112 */
        {  ERROR_INVALID_TARGET_HANDLE,  osl_File_E_BADF     },  /* 114 */
        {  ERROR_INVALID_HANDLE,         osl_File_E_INVAL    },  /* 124 */
        {  ERROR_WAIT_NO_CHILDREN,       osl_File_E_CHILD    },  /* 128 */
        {  ERROR_CHILD_NOT_COMPLETE,     osl_File_E_CHILD    },  /* 129 */
        {  ERROR_DIRECT_ACCESS_HANDLE,   osl_File_E_BADF     },  /* 130 */
        {  ERROR_NEGATIVE_SEEK,          osl_File_E_INVAL    },  /* 131 */
        {  ERROR_SEEK_ON_DEVICE,         osl_File_E_ACCES    },  /* 132 */
        {  ERROR_DIR_NOT_EMPTY,          osl_File_E_NOTEMPTY },  /* 145 */
        {  ERROR_NOT_LOCKED,             osl_File_E_ACCES    },  /* 158 */
        {  ERROR_BAD_PATHNAME,           osl_File_E_NOENT    },  /* 161 */
        {  ERROR_MAX_THRDS_REACHED,      osl_File_E_AGAIN    },  /* 164 */
        {  ERROR_LOCK_FAILED,            osl_File_E_ACCES    },  /* 167 */
        {  ERROR_ALREADY_EXISTS,         osl_File_E_EXIST    },  /* 183 */
        {  ERROR_FILENAME_EXCED_RANGE,   osl_File_E_NOENT    },  /* 206 */
        {  ERROR_NESTING_NOT_ALLOWED,    osl_File_E_AGAIN    },  /* 215 */
        {  ERROR_NOT_ENOUGH_QUOTA,       osl_File_E_NOMEM    }    /* 1816 */
    };

    /* size of the table */
    #define ERRTABLESIZE (sizeof(errtable)/sizeof(errtable[0]))

    /* The following two constants must be the minimum and maximum
    values in the (contiguous) range of osl_File_E_xec Failure errors. */
    #define MIN_EXEC_ERROR ERROR_INVALID_STARTING_CODESEG
    #define MAX_EXEC_ERROR ERROR_INFLOOP_IN_RELOC_CHAIN

    /* These are the low and high value in the range of errors that are
    access violations */
    #define MIN_EACCES_RANGE ERROR_WRITE_PROTECT
    #define MAX_EACCES_RANGE ERROR_SHARING_BUFFER_EXCEEDED


    /*******************************************************************************/

    oslFileError _mapError( DWORD dwError )
    {
        int i;

        /* check the table for the OS error code */
        for ( i = 0; i < ERRTABLESIZE; ++i )
        {
            if ( dwError == errtable[i].oscode )
                return (oslFileError)errtable[i].errnocode;
        }

        /* The error code wasn't in the table.  We check for a range of */
        /* osl_File_E_ACCES errors or exec failure errors (ENOEXEC).  Otherwise   */
        /* osl_File_E_INVAL is returned.                                          */

        if ( dwError >= MIN_EACCES_RANGE && dwError <= MAX_EACCES_RANGE)
            return osl_File_E_ACCES;
        else if ( dwError >= MIN_EXEC_ERROR && dwError <= MAX_EXEC_ERROR)
            return osl_File_E_NOEXEC;
        else
            return osl_File_E_INVAL;
    }

    #define MapError( oserror ) _mapError( oserror )

    #define E_UNKNOWN_EXEC_ERROR -1

} // end namespace

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------

CSysShExec::CSysShExec( ) :
    WeakComponentImplHelper2< XSystemShellExecute, XServiceInfo >( m_aMutex )
{
}

//-------------------------------------------------
//
//-------------------------------------------------

void SAL_CALL CSysShExec::execute( const OUString& aCommand, const OUString& aParameter, sal_Int32 nFlags )
        throw (IllegalArgumentException, SystemShellExecuteException, RuntimeException)
{
    // parameter checking
    if ( !aCommand.getLength( ) )
        throw IllegalArgumentException(
            OUString::createFromAscii( "Empty command" ),
            static_cast< XSystemShellExecute* >( this ),
            1 );

    if ( !( nFlags >= SSEF_DEFAULTS && nFlags <= SSEF_NO_SYSTEM_ERROR_MESSAGE ) )
        throw IllegalArgumentException(
            OUString::createFromAscii( "Invalid Flags specified" ),
            static_cast< XSystemShellExecute* >( this ),
            3 );

    SHELLEXECUTEINFOW sei;
    ZeroMemory( &sei, sizeof( sei ) );

    sei.cbSize       = sizeof( sei );
    sei.lpFile       = aCommand.getStr();
    sei.lpParameters = aParameter.getStr();
    sei.nShow        = SW_SHOWNORMAL;

    if ( SSEF_NO_SYSTEM_ERROR_MESSAGE & nFlags )
        sei.fMask = SEE_MASK_FLAG_NO_UI;

    SetLastError( 0 );

    if ( !ShellExecuteExW( &sei ) )
    {
        // ShellExecuteEx fails to set an error code
        // we return osl_File_E_INVAL
        sal_Int32 psxErr = GetLastError( );
        if ( ERROR_SUCCESS == psxErr )
            psxErr = E_UNKNOWN_EXEC_ERROR;
        else
            psxErr = MapError( psxErr );

        throw SystemShellExecuteException(
            OUString::createFromAscii( "Error executing command" ),
            static_cast< XSystemShellExecute* >( this ),
            psxErr );
    }
}

// -------------------------------------------------
// XServiceInfo
// -------------------------------------------------

OUString SAL_CALL CSysShExec::getImplementationName(  )
    throw( RuntimeException )
{
    return OUString::createFromAscii( SYSSHEXEC_IMPL_NAME );
}

// -------------------------------------------------
//  XServiceInfo
// -------------------------------------------------

sal_Bool SAL_CALL CSysShExec::supportsService( const OUString& ServiceName )
    throw( RuntimeException )
{
    Sequence < OUString > SupportedServicesNames = SysShExec_getSupportedServiceNames();

    for ( sal_Int32 n = SupportedServicesNames.getLength(); n--; )
        if (SupportedServicesNames[n].compareTo(ServiceName) == 0)
            return sal_True;

    return sal_False;
}

// -------------------------------------------------
//  XServiceInfo
// -------------------------------------------------

Sequence< OUString > SAL_CALL CSysShExec::getSupportedServiceNames(  )
    throw( RuntimeException )
{
    return SysShExec_getSupportedServiceNames();
}

