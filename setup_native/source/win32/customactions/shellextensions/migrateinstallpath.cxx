/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: migrateinstallpath.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: vg $ $Date: 2007-03-26 14:11:29 $
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

#pragma warning(push, 1) /* disable warnings within system headers */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msiquery.h>
#pragma warning(pop)

#include <malloc.h>

#ifdef UNICODE
#define _UNICODE
#define _tstring    wstring
#else
#define _tstring    string
#endif
#include <tchar.h>
#include <string>

#if defined(_WIN32_WINNT) && !defined(__MINGW32__)
#error YES
#endif

using namespace std;

namespace
{
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
} // namespace

extern "C" UINT __stdcall MigrateInstallPath( MSIHANDLE handle )
{
    TCHAR   szValue[8192];
    DWORD   nValueSize = sizeof(szValue);
    HKEY    hKey;
    std::_tstring   sInstDir;

    std::_tstring   sManufacturer = GetMsiProperty( handle, TEXT("Manufacturer") );
    std::_tstring   sDefinedName = GetMsiProperty( handle, TEXT("DEFINEDPRODUCT") );
    std::_tstring   sUpdateVersion = GetMsiProperty( handle, TEXT("DEFINEDVERSION") );
    std::_tstring   sUpgradeCode = GetMsiProperty( handle, TEXT("UpgradeCode") );

    std::_tstring   sProductKey = "Software\\" + sManufacturer + "\\" + sDefinedName +
                                        "\\" + sUpdateVersion + "\\" + sUpgradeCode;

    std::_tstring   mystr;
    mystr = "ProductKey: " + sProductKey;
    // MessageBox( NULL, mystr.c_str(), "ProductKey", MB_OK );

    if ( ERROR_SUCCESS == RegOpenKey( HKEY_CURRENT_USER,  sProductKey.c_str(), &hKey ) )
    {
        if ( ERROR_SUCCESS == RegQueryValueEx( hKey, TEXT("INSTALLLOCATION"), NULL, NULL, (LPBYTE)szValue, &nValueSize ) )
        {
            sInstDir = szValue;
            MsiSetProperty(handle, TEXT("INSTALLLOCATION"), sInstDir.c_str());
            // MessageBox( NULL, sInstDir.c_str(), "Found in HKEY_CURRENT_USER", MB_OK );
        }
        RegCloseKey( hKey );
    }
    else if ( ERROR_SUCCESS == RegOpenKey( HKEY_LOCAL_MACHINE,  sProductKey.c_str(), &hKey ) )
    {
        if ( ERROR_SUCCESS == RegQueryValueEx( hKey, TEXT("INSTALLLOCATION"), NULL, NULL, (LPBYTE)szValue, &nValueSize ) )
        {
            sInstDir = szValue;
            MsiSetProperty(handle, TEXT("INSTALLLOCATION"), sInstDir.c_str());
            // MessageBox( NULL, sInstDir.c_str(), "Found in HKEY_LOCAL_MACHINE", MB_OK );
        }
        RegCloseKey( hKey );
    }

    return ERROR_SUCCESS;

}
