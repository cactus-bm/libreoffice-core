/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#define _WIN32_WINDOWS 0x0410

#ifdef _MSC_VER
#pragma warning(push, 1) /* disable warnings within system headers */
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msiquery.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <malloc.h>
#include <assert.h>

#ifdef UNICODE
#define _UNICODE
#define _tstring    wstring
#else
#define _tstring    string
#endif
#include <tchar.h>
#include <string>
#include <queue>
#include <stdio.h>
#include <sal/macros.h>

#include <systools/win32/uwinapi.h>
#include <../tools/seterror.hxx>

#define WININIT_FILENAME    "wininit.ini"
#define RENAME_SECTION      "rename"

#ifdef DEBUG
inline void OutputDebugStringFormat( LPCTSTR pFormat, ... )
{
    _TCHAR  buffer[1024];
    va_list args;

    va_start( args, pFormat );
    _vsntprintf( buffer, SAL_N_ELEMENTS(buffer), pFormat, args );
    OutputDebugString( buffer );
}
#else
static inline void OutputDebugStringFormat( LPCTSTR, ... )
{
}
#endif

static std::_tstring GetMsiProperty( MSIHANDLE handle, const std::_tstring& sProperty )
{
    std::_tstring   result;
    TCHAR   szDummy[1] = TEXT("");
    DWORD   nChars = 0;

    if ( MsiGetProperty( handle, sProperty.c_str(), szDummy, &nChars ) == ERROR_MORE_DATA )
    {
        DWORD nBytes = ++nChars * sizeof(TCHAR);
        LPTSTR buffer = reinterpret_cast<LPTSTR>(_alloca(nBytes));
        ZeroMemory( buffer, nBytes );
        MsiGetProperty(handle, sProperty.c_str(), buffer, &nChars);
        result = buffer;
    }

    return  result;
}

static inline bool IsSetMsiProperty(MSIHANDLE handle, const std::_tstring& sProperty)
{
    std::_tstring value = GetMsiProperty(handle, sProperty);
    return (value.length() > 0);
}

static inline void UnsetMsiProperty(MSIHANDLE handle, const std::_tstring& sProperty)
{
    MsiSetProperty(handle, sProperty.c_str(), NULL);
}

static inline void SetMsiProperty(MSIHANDLE handle, const std::_tstring& sProperty)
{
    MsiSetProperty(handle, sProperty.c_str(), TEXT("1"));
}

extern "C" UINT __stdcall IsOfficeRunning( MSIHANDLE handle )
{
    OSVERSIONINFO   osverinfo;
    osverinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx( &osverinfo );

    // renaming the vcl resource doesn't work reliable with OS >= Windows Vista
    if (osverinfo.dwMajorVersion < 6 )
    {
        std::_tstring sInstDir = GetMsiProperty( handle, TEXT("INSTALLLOCATION") );
        // Property empty -> no office installed
        if ( sInstDir.length() == 0 )
            return ERROR_SUCCESS;

        std::_tstring sResourceDir = sInstDir + TEXT("Basis\\program\\resource\\");
        std::_tstring sPattern = sResourceDir + TEXT("vcl*.res");

//        std::_tstring mystr;
//        mystr = "IsOfficeRunning start. Checking file in dir: " + sResourceDir;
//        MessageBox( NULL, mystr.c_str(), "IsOfficeRunning", MB_OK );

        WIN32_FIND_DATA aFindFileData;
        HANDLE  hFind = FindFirstFile( sPattern.c_str(), &aFindFileData );

        if ( IsValidHandle(hFind) )
        {
            BOOL    fSuccess = false;
            bool    fRenameSucceeded;

            do
            {
                std::_tstring   sResourceFile = sResourceDir + aFindFileData.cFileName;
                std::_tstring   sIntermediate = sResourceFile + TEXT(".tmp");

                fRenameSucceeded = MoveFileExA( sResourceFile.c_str(), sIntermediate.c_str(), MOVEFILE_REPLACE_EXISTING );
                if ( fRenameSucceeded )
                {
                    MoveFileExA( sIntermediate.c_str(), sResourceFile.c_str(), 0 );
                    fSuccess = FindNextFile( hFind, &aFindFileData );
                }
            } while ( fSuccess && fRenameSucceeded );

            if ( !fRenameSucceeded )
            {
                MsiSetProperty(handle, TEXT("OFFICERUNS"), TEXT("1"));
                SetMsiErrorCode( MSI_ERROR_OFFICE_IS_RUNNING );

//                mystr = "Office is running";
//                MessageBox( NULL, mystr.c_str(), "IsOfficeRunning", MB_OK );
            }

            FindClose( hFind );
        }
//        mystr = "IsOfficeRunning end";
//        MessageBox( NULL, mystr.c_str(), "IsOfficeRunning", MB_OK );
    }
    else
    {
        std::_tstring sOfficeInstallPath = GetMsiProperty(handle, TEXT("INSTALLLOCATION"));
        // Property empty -> no office installed
        if ( sOfficeInstallPath.length() == 0 )
            return ERROR_SUCCESS;

        std::_tstring sRenameSrc = sOfficeInstallPath + TEXT("program");
        std::_tstring sRenameDst = sOfficeInstallPath + TEXT("program_test");

        bool bSuccess = MoveFile( sRenameSrc.c_str(), sRenameDst.c_str() );

        if ( bSuccess )
        {
            MoveFile( sRenameDst.c_str(), sRenameSrc.c_str() );
        }
        else
        {
            DWORD  dwError = GetLastError();
            LPVOID lpMsgBuf;
            // When there is no program folder, there could be no running office
            if ( dwError == ERROR_FILE_NOT_FOUND )
                return ERROR_SUCCESS;
            if ( dwError == ERROR_PATH_NOT_FOUND )
                return ERROR_SUCCESS;

            // The destination folder should never exist, don't know what to do here
            if ( dwError == ERROR_ALREADY_EXISTS )
                return ERROR_SUCCESS;

            if ( FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPTSTR) &lpMsgBuf,
                0,
                NULL ))
            {
                OutputDebugStringFormat( TEXT("Error Code %d: %s"), dwError, lpMsgBuf );
                LocalFree( lpMsgBuf );
            }
            else
                OutputDebugStringFormat( TEXT("Error Code %d: Unknown"), dwError );

            MsiSetProperty( handle, TEXT("OFFICERUNS"), TEXT("1") );
            SetMsiErrorCode( MSI_ERROR_OFFICE_IS_RUNNING );
        }
    }

    return ERROR_SUCCESS;
}



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
