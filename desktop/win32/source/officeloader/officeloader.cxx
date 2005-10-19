/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: officeloader.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-10-19 12:20:52 $
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
#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

#define _UNICODE
#include <tchar.h>

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <systools/win32/uwinapi.h>

#include "../../../source/inc/exithelper.hxx"

#define PIPE_PREFIX                 TEXT("\\\\.\\pipe\\OSL_PIPE_")
#define PIPE_POSTFIX                TEXT("_SingleOfficeIPC_")
#define PIPE_TERMINATION_SEQUENCE   "InternalIPC::ProcessingDone"

BOOL WINAPI ConvertSidToStringSid( PSID pSid, LPTSTR* StringSid )
{
    PSID_IDENTIFIER_AUTHORITY psia;
    DWORD dwSubAuthorities;
    DWORD dwSidRev=SID_REVISION;
    DWORD dwCounter;
    DWORD dwSidSize;

    // Validate the binary SID.

    if(!IsValidSid(pSid)) return FALSE;

    // Get the identifier authority value from the SID.

    psia = GetSidIdentifierAuthority(pSid);

    // Get the number of subauthorities in the SID.

    dwSubAuthorities = *GetSidSubAuthorityCount(pSid);

    // Compute the buffer length.
    // S-SID_REVISION- + IdentifierAuthority- + subauthorities- + NULL

    dwSidSize=(15 + 12 + (12 * dwSubAuthorities) + 1) * sizeof(TCHAR);

    *StringSid = (LPTSTR)LocalAlloc( LMEM_FIXED, dwSidSize );

    // Add 'S' prefix and revision number to the string.

    dwSidSize=wsprintf(*StringSid, TEXT("S-%lu-"), dwSidRev );

    // Add a SID identifier authority to the string.

    if ( (psia->Value[0] != 0) || (psia->Value[1] != 0) )
    {
        dwSidSize+=wsprintf(*StringSid + lstrlen(*StringSid),
                    TEXT("0x%02hx%02hx%02hx%02hx%02hx%02hx"),
                    (USHORT)psia->Value[0],
                    (USHORT)psia->Value[1],
                    (USHORT)psia->Value[2],
                    (USHORT)psia->Value[3],
                    (USHORT)psia->Value[4],
                    (USHORT)psia->Value[5]);
    }
    else
    {
        dwSidSize+=wsprintf(*StringSid + lstrlen(*StringSid),
                    TEXT("%lu"),
                    (ULONG)(psia->Value[5]      )   +
                    (ULONG)(psia->Value[4] <<  8)   +
                    (ULONG)(psia->Value[3] << 16)   +
                    (ULONG)(psia->Value[2] << 24)   );
    }

    // Add SID subauthorities to the string.
    //
    for (dwCounter=0 ; dwCounter < dwSubAuthorities ; dwCounter++)
    {
        dwSidSize+=wsprintf(*StringSid + dwSidSize, TEXT("-%lu"),
                    *GetSidSubAuthority(pSid, dwCounter) );
    }

    return TRUE;
}


//---------------------------------------------------------------------------

int WINAPI _tWinMain( HINSTANCE, HINSTANCE, LPTSTR, int )
{
    TCHAR               szTargetFileName[MAX_PATH] = TEXT("");
    TCHAR               szPerfTuneIniFile[MAX_PATH] = TEXT("");
    STARTUPINFO         aStartupInfo;

    ZeroMemory( &aStartupInfo, sizeof(aStartupInfo) );
    aStartupInfo.cb = sizeof(aStartupInfo);

    GetStartupInfo( &aStartupInfo );
    // Get image path with same name but with .bin extension

    TCHAR               szModuleFileName[MAX_PATH];

    GetModuleFileName( NULL, szModuleFileName, MAX_PATH );
    _TCHAR  *lpLastDot = _tcsrchr( szModuleFileName, '.' );
    if ( lpLastDot && 0 == _tcsicmp( lpLastDot, _T(".EXE") ) )
    {
        size_t len = lpLastDot - szModuleFileName;
        _tcsncpy( szTargetFileName, szModuleFileName, len );
        _tcsncpy( szTargetFileName + len, _T(".BIN"), sizeof(szTargetFileName)/sizeof(szTargetFileName[0]) - len );
    }

    _TCHAR  *lpLastSlash = _tcsrchr( szModuleFileName, '\\' );
    if ( lpLastSlash )
    {
        size_t len = lpLastSlash - szModuleFileName + 1;
        _tcsncpy( szPerfTuneIniFile, szModuleFileName, len );
        _tcsncpy( szPerfTuneIniFile + len, _T("perftune.ini"), sizeof(szPerfTuneIniFile)/sizeof(szPerfTuneIniFile[0]) - len );
    }

    // Create process with same command line, environment and stdio handles which
    // are directed to the created pipes

    DWORD   dwExitCode = (DWORD)-1;
    BOOL    fSuccess = FALSE;

    do
    {
        TCHAR   szKey[32];

        GetPrivateProfileString(
            TEXT("PerformanceTuning"),
            TEXT("FastPipeCommunication"),
            TEXT("0"),
            szKey,
            elementsof(szKey),
            szPerfTuneIniFile
            );

        if ( 0 == _tcscmp( szKey, TEXT("1") ) )
        {
            HANDLE  hProcessToken;

            if ( OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hProcessToken ) )
            {
                TCHAR   szPipeName[4096];


                DWORD   dwTokenLength = 0;


                fSuccess = GetTokenInformation( hProcessToken, TokenUser, NULL, dwTokenLength, &dwTokenLength );

                PVOID   pTokenInfo = _alloca(dwTokenLength);
                fSuccess = GetTokenInformation( hProcessToken, TokenUser, pTokenInfo, dwTokenLength, &dwTokenLength );
                CloseHandle( hProcessToken );

                PSID pSid = ((PTOKEN_USER)pTokenInfo)->User.Sid;
                LPTSTR  szUserIdent = NULL;
                TCHAR   szSUPD[11] = TEXT("0");

                fSuccess = ConvertSidToStringSid( pSid, &szUserIdent );

                _tcsncpy( szPipeName, PIPE_PREFIX, elementsof(szPipeName) );
                _tcsncat( szPipeName, szUserIdent, elementsof(szPipeName) - _tcslen(szPipeName) - 1 );
                _tcsncat( szPipeName, PIPE_POSTFIX, elementsof(szPipeName) - _tcslen(szPipeName) - 1 );
                _tcsncat( szPipeName, _ultot( SUPD, szSUPD, 10), elementsof(szPipeName) - _tcslen(szPipeName) - 1 );

                LocalFree( szUserIdent );

                HANDLE  hPipe = CreateFile(
                                    szPipeName,
                                    GENERIC_READ|GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);

                if ( INVALID_HANDLE_VALUE != hPipe )
                {
                    DWORD   dwBytesWritten;
                    int argc = 0;
                    LPWSTR  *argv = CommandLineToArgvW( GetCommandLine(), &argc );

                    fSuccess = TRUE;
                    for ( int argn = 1; fSuccess && argn < argc; argn++ )
                    {
                        CHAR    szBuffer[4096];

                        WideCharToMultiByte( CP_ACP, 0, argv[argn], -1, szBuffer, sizeof(szBuffer), NULL, NULL );
                        if ( argn + 1 < argc )
                            strcat( szBuffer, "|" );
                        fSuccess = WriteFile(  hPipe, szBuffer, strlen(szBuffer), &dwBytesWritten, NULL );
                    }

                    if ( fSuccess )
                    {
                        fSuccess = WriteFile(  hPipe, "", 1, &dwBytesWritten, NULL );
                        if ( fSuccess )
                        {
                            DWORD   dwBytesRead = 0;
                            char    *pBuffer = (char *)_alloca( sizeof(PIPE_TERMINATION_SEQUENCE) );
                            fSuccess = ReadFile( hPipe, pBuffer, sizeof(PIPE_TERMINATION_SEQUENCE) - 1, &dwBytesRead, NULL );
                            if ( fSuccess )
                            {
                                pBuffer[dwBytesRead] = 0;
                                if ( 0 != strcmp( PIPE_TERMINATION_SEQUENCE, pBuffer ) )
                                    fSuccess = FALSE;
                            }
                        }
                    }

                    CloseHandle( hPipe );

                    return fSuccess ? 0 : -1;
                }

            }
        }

        PROCESS_INFORMATION aProcessInfo;

        fSuccess = FALSE;
        fSuccess = CreateProcess(
            szTargetFileName,
            // When restarting office process do not pass a command line
            fSuccess ? NULL : GetCommandLine(),
            NULL,
            NULL,
            TRUE,
            0,
            NULL,
            NULL,
            &aStartupInfo,
            &aProcessInfo );

        if ( fSuccess )
        {
            DWORD   dwWaitResult;

            do
            {
                // On Windows XP it seems as the desktop calls WaitForInputIdle after "OpenWidth" so we have to do so
                // as if we where processing any messages

                dwWaitResult = MsgWaitForMultipleObjects( 1, &aProcessInfo.hProcess, FALSE, INFINITE, QS_ALLEVENTS );

                if (  WAIT_OBJECT_0 + 1 == dwWaitResult )
                {
                    MSG msg;

                    PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
                }
            } while ( WAIT_OBJECT_0 + 1 == dwWaitResult );

            dwExitCode = 0;
            GetExitCodeProcess( aProcessInfo.hProcess, &dwExitCode );

            CloseHandle( aProcessInfo.hProcess );
            CloseHandle( aProcessInfo.hThread );
        }
    } while ( fSuccess && ::desktop::ExitHelper::E_CRASH_WITH_RESTART == dwExitCode );

    return fSuccess ? dwExitCode : -1;
}
