/*************************************************************************
 *
 *  $RCSfile: registry.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2004-04-07 11:17:53 $
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

#include <windows.h>
#include <malloc.h>

#ifndef DBGMACROS_HXX_INCLUDED
#include "internal/dbgmacros.hxx"
#endif

#ifndef REGISTRY_HXX_INCLUDED
#include "internal/registry.hxx"
#endif

#include <objbase.h>

//---------------------------------------
//
//---------------------------------------

// Size of a CLSID as a string
const int CLSID_STRING_SIZE = 39;

//---------------------------------------
//
//---------------------------------------

bool SetRegistryKey(HKEY RootKey, const char* KeyName, const char* ValueName, const char* Value)
{
    HKEY hSubKey;

    // open or create the desired key
    int rc = RegCreateKeyExA(
        RootKey, KeyName, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &hSubKey, 0);

    if (ERROR_SUCCESS == rc)
    {
        rc = RegSetValueExA(
            hSubKey, ValueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(Value), strlen(Value) + 1);

        RegCloseKey(hSubKey);
    }

    return (ERROR_SUCCESS == rc);
}

//---------------------------------------
//
//---------------------------------------

bool DeleteRegistryKey(HKEY RootKey, const char* KeyName)
{
    HKEY hKey;

    int rc = RegOpenKeyExA(
        RootKey,
        KeyName,
        0,
        KEY_READ | DELETE,
        &hKey);

    if (ERROR_SUCCESS == rc)
    {
        char* SubKey;
        DWORD nMaxSubKeyLen;

        rc = RegQueryInfoKeyA(
            hKey, 0, 0, 0, 0,
            &nMaxSubKeyLen,
            0, 0, 0, 0, 0, 0);

        nMaxSubKeyLen++; // space for trailing '\0'

        SubKey = reinterpret_cast<char*>(
            _alloca(nMaxSubKeyLen*sizeof(char)));

        while (ERROR_SUCCESS == rc)
        {
            DWORD nLen = nMaxSubKeyLen;

            rc = RegEnumKeyExA(
                hKey,
                0,       // always index zero
                SubKey,
                &nLen,
                0, 0, 0, 0);

            if (ERROR_NO_MORE_ITEMS == rc)
            {
                rc = RegDeleteKeyA(RootKey, KeyName);
                break;
            }
            else if (rc == ERROR_SUCCESS)
            {
                DeleteRegistryKey(hKey, SubKey);
            }

        } // while

        RegCloseKey(hKey);

    } // if

    return (ERROR_SUCCESS == rc);
}

/** May be used to determine if the specified registry key has subkeys
    The function returns true on success else if an error occures false
*/
bool HasSubkeysRegistryKey(HKEY RootKey, const char* KeyName, /* out */ bool& bResult)
{
    HKEY hKey;

    LONG rc = RegOpenKeyExA(RootKey, KeyName, 0, KEY_READ, &hKey);

    if (ERROR_SUCCESS == rc)
    {
        DWORD nSubKeys = 0;

        rc = RegQueryInfoKeyA(hKey, 0, 0, 0, &nSubKeys, 0, 0, 0, 0, 0, 0, 0);

        bResult = (nSubKeys > 0);
    }

    return (ERROR_SUCCESS == rc);
}

// Convert a CLSID to a char string.
std::string ClsidToString(const CLSID& clsid)
{
    // Get CLSID
    LPOLESTR wszCLSID = NULL;
    HRESULT hr = StringFromCLSID(clsid, &wszCLSID);

    char buff[39];
    // Covert from wide characters to non-wide.
    wcstombs(buff, wszCLSID, sizeof(buff));

    // Free memory.
    CoTaskMemFree(wszCLSID) ;

    return std::string(buff);
}

//---------------------------------------
//
//---------------------------------------

bool QueryRegistryKey(HKEY RootKey, const char* KeyName, char szProductType[], DWORD dwBufLen)
{
    HKEY hKey;

    int rc = RegOpenKeyExA(
        RootKey,
        KeyName,
        0,
        KEY_READ,
        &hKey);

    if (ERROR_SUCCESS == rc)
    {
        rc = RegQueryValueExA(
            hKey, NULL, NULL, NULL, (LPBYTE)szProductType,&dwBufLen);

        RegCloseKey(hKey);
    }

    return (ERROR_SUCCESS == rc);
}
