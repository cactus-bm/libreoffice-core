/*************************************************************************
 *
 *  $RCSfile: startmenuicon.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2005-03-23 09:40:58 $
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msiquery.h>
#include <malloc.h>

#ifdef UNICODE
#define _UNICODE
#define _tstring    wstring
#else
#define _tstring    string
#endif
#include <tchar.h>
#include <string>

#ifdef _WIN32_WINNT
#error YES
#endif

std::_tstring GetMsiProperty( MSIHANDLE handle, const std::_tstring& sProperty )
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

/*
    Called during installation to customize the start menu folder icon.
    See: http://msdn.microsoft.com/library/en-us/shellcc/platform/shell/programmersguide/shell_basics/shell_basics_extending/custom.asp
*/
extern "C" UINT __stdcall InstallStartmenuFolderIcon( MSIHANDLE handle )
{
    std::_tstring   sOfficeMenuFolder = GetMsiProperty( handle, TEXT("OfficeMenuFolder") );
    std::_tstring sDesktopFile = sOfficeMenuFolder + TEXT("Desktop.ini");

//    MessageBox(NULL, sDesktopFile.c_str(), TEXT("OfficeMenuFolder"), MB_OK | MB_ICONINFORMATION);

    std::_tstring   sIconFile = GetMsiProperty( handle, TEXT("INSTALLLOCATION") ) + TEXT("program\\soffice.exe");

    WritePrivateProfileString(
        TEXT(".ShellClassInfo"),
        TEXT("IconFile"),
        sIconFile.c_str(),
        sDesktopFile.c_str() );

    WritePrivateProfileString(
        TEXT(".ShellClassInfo"),
        TEXT("IconIndex"),
        TEXT("18"),
        sDesktopFile.c_str() );

    WritePrivateProfileString(
        TEXT(".ShellClassInfo"),
        TEXT("ConfirmFileOp"),
        TEXT("0"),
        sDesktopFile.c_str() );

    /*
    WritePrivateProfileString(
        TEXT(".ShellClassInfo"),
        TEXT("InfoTip"),
        TEXT("StarOffice Productivity Suite"),
        sDesktopFile.c_str() );
        */

    SetFileAttributes( sDesktopFile.c_str(), FILE_ATTRIBUTE_HIDDEN );
    SetFileAttributes( sOfficeMenuFolder.c_str(), FILE_ATTRIBUTE_SYSTEM );

    return ERROR_SUCCESS;
}

extern "C" UINT __stdcall DeinstallStartmenuFolderIcon(MSIHANDLE handle)
{
    std::_tstring   sOfficeMenuFolder = GetMsiProperty( handle, TEXT("OfficeMenuFolder") );
    std::_tstring sDesktopFile = sOfficeMenuFolder + TEXT("Desktop.ini");

    SetFileAttributes( sDesktopFile.c_str(), FILE_ATTRIBUTE_NORMAL );
    DeleteFile( sDesktopFile.c_str() );

    SetFileAttributes( sOfficeMenuFolder.c_str(), FILE_ATTRIBUTE_NORMAL );

    return ERROR_SUCCESS;
}
