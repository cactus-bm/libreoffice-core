/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _SYMBOL_LOADER_HXX
#define _SYMBOL_LOADER_HXX
#if defined( WNT )
# include <windows.h>
# include <winreg.h>
#endif
#include <iostream>
#include <osl/module.h>
#include <rtl/ustring.hxx>

#define SYM_MAP(a) { #a, (SymbolFunc *)&a }

namespace avmedia
{
namespace vlc
{
namespace wrapper
{
typedef void (*SymbolFunc) (void);

struct ApiMap
{
    const char *symName;
    SymbolFunc *refValue;
};

namespace
{
#if defined( UNX )
    const char LibName[] = "libvlc.so.5";
#elif defined( MACOSX )
    const char LibName[] = "/Applications/VLC.app/Contents/MacOS/lib/libvlc.dylib";
#elif defined( WNT )
    const char LibName[] = "libvlc.dll";

    OUString GetVLCPath()
    {
        HKEY hKey;
        wchar_t arCurrent[MAX_PATH];
        DWORD dwType, dwCurrentSize = sizeof( arCurrent );

        if ( ::RegOpenKeyExW( HKEY_LOCAL_MACHINE, L"Software\\VideoLAN\\VLC",
                             0, KEY_READ, &hKey ) == ERROR_SUCCESS )
        {
            if ( ::RegQueryValueExW( hKey, L"InstallDir", NULL, &dwType, (LPBYTE) arCurrent, &dwCurrentSize ) == ERROR_SUCCESS &&
                 dwType == REG_SZ )
            {
                ::RegCloseKey( hKey );
                // The value might be 0-terminated or not
                if (arCurrent[dwCurrentSize/2] == 0)
                    dwCurrentSize -= 2;
                return OUString( arCurrent, dwCurrentSize ) + "/";
            }

            ::RegCloseKey( hKey );
        }
        return OUString();
    }
#endif

    template<size_t N>
    bool tryLink( oslModule &aModule, const ApiMap ( &pMap )[N] )
    {
        for (size_t i = 0; i < N; ++i)
        {
            SymbolFunc aMethod = ( SymbolFunc )osl_getFunctionSymbol
                ( aModule, OUString::createFromAscii( pMap[ i ].symName ).pData );
            if ( !aMethod )
            {
                SAL_WARN("avmedia", "Cannot load method " << pMap[ i ].symName);
                *pMap[ i ].refValue = NULL;
                return false;
            }
            else
                *pMap[ i ].refValue = aMethod;
        }

        return true;
    }
}

    template<size_t N>
    bool InitApiMap( const ApiMap ( &pMap )[N]  )
    {
#if defined( UNX ) || defined( MACOSX )
        const OUString& fullPath = OUString::createFromAscii(LibName);
#elif defined( WNT )
        const OUString& fullPath = GetVLCPath() + OUString::createFromAscii(LibName);
#endif

        oslModule aModule = osl_loadModule( fullPath.pData,
                                            SAL_LOADMODULE_DEFAULT );

        if( aModule == NULL)
        {
            SAL_WARN("avmedia", "Cannot load libvlc");
            return false;
        }

        if (tryLink( aModule, pMap ))
        {
            return true;
        }

        osl_unloadModule( aModule );

        return false;
    }
}
}
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
