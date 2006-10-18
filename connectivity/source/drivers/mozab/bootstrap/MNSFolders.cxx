/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: MNSFolders.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: ihi $ $Date: 2006-10-18 13:10:09 $
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
#include "precompiled_connectivity.hxx"
#ifndef _MNSFOLDERS_HXX_
#include <MNSFolders.hxx>
#endif

#ifdef UNIX
#include <sys/types.h>
#include <strings.h>
#include <string.h>
#endif // End UNIX

#ifdef WNT
#include "pre_include_windows.h"
#include <windows.h>
#include <stdlib.h>
#include <shlobj.h>
#include <objidl.h>
#include "post_include_windows.h"
#endif // End WNT

#ifndef _OSL_SECURITY_HXX_
#include <osl/security.hxx>
#endif
#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif
#ifndef _OSL_THREAD_H_
#include <osl/thread.h>
#endif

using namespace ::com::sun::star::mozilla;

namespace
{
    #if defined(XP_MAC) || defined(XP_MACOSX)
        #define APP_REGISTRY_NAME "Application Registry"
    #elif defined(XP_WIN) || defined(XP_OS2)
        #define APP_REGISTRY_NAME "registry.dat"
    #else
        #define APP_REGISTRY_NAME "appreg"
    #endif

    // -------------------------------------------------------------------
    static ::rtl::OUString lcl_getUserDataDirectory()
    {
        ::osl::Security   aSecurity;
        ::rtl::OUString   aConfigPath;

        aSecurity.getConfigDir( aConfigPath );
        return aConfigPath + ::rtl::OUString::createFromAscii( "/" );
    }

    // -------------------------------------------------------------------
    static const char* DefaultProductDir[3][3] =
    {
    #if defined(XP_WIN)
        { "Mozilla/", NULL, NULL },
        { "Mozilla/Firefox/", NULL, NULL },
        { "Thunderbird/", "Mozilla/Thunderbird/", NULL }
    #else
        { ".mozilla/", NULL, NULL },
        { ".mozilla/firefox/", NULL, NULL },
        { ".thunderbird/", ".mozilla-thunderbird/", ".mozilla/thunderbird/" }
    #endif
    };

    static const char* ProductRootEnvironmentVariable[3] =
    {
        "MOZILLA_PROFILE_ROOT",
        "MOZILLA_FIREFOX_PROFILE_ROOT",
        "MOZILLA_THUNDERBIRD_PROFILE_ROOT",
    };

    // -------------------------------------------------------------------
    static ::rtl::OUString lcl_guessProfileRoot( MozillaProductType _product )
    {
        size_t productIndex = _product - 1;

        static ::rtl::OUString s_productDirectories[3];

        if ( !s_productDirectories[ productIndex ].getLength() )
        {
            ::rtl::OUString sProductPath;

            // check whether we have an anevironment variable which helps us
            const char* pProfileByEnv = getenv( ProductRootEnvironmentVariable[ productIndex ] );
            if ( pProfileByEnv )
            {
                sProductPath = ::rtl::OUString( pProfileByEnv, strlen( pProfileByEnv ), osl_getThreadTextEncoding() );
                // asume that this is fine, no further checks
            }
            else
            {
                ::rtl::OUString sProductDirCandidate;
                const char* pProfileRegistry = ( _product == MozillaProductType_Mozilla ) ? APP_REGISTRY_NAME : "profiles.ini";

                // check all possible candidates
                for ( size_t i=0; i<3; ++i )
                {
                    if ( NULL == DefaultProductDir[ productIndex ][ i ] )
                        break;

                    sProductDirCandidate = lcl_getUserDataDirectory() +
                        ::rtl::OUString::createFromAscii( DefaultProductDir[ productIndex ][ i ] );

                    // check existence
                    ::osl::DirectoryItem aRegistryItem;
                    ::osl::FileBase::RC result = ::osl::DirectoryItem::get( sProductDirCandidate + ::rtl::OUString::createFromAscii( pProfileRegistry ), aRegistryItem );
                    if ( result == ::osl::FileBase::E_None  )
                    {
                        ::osl::FileStatus aStatus( FileStatusMask_Validate );
                        result = aRegistryItem.getFileStatus( aStatus );
                        if ( result == ::osl::FileBase::E_None  )
                        {
                            // the registry file exists
                            break;
                        }
                    }
                }

                ::osl::FileBase::getSystemPathFromFileURL( sProductDirCandidate, sProductPath );
            }

            s_productDirectories[ productIndex ] = sProductPath;
        }

        return s_productDirectories[ productIndex ];
    }
}

// -----------------------------------------------------------------------
::rtl::OUString getRegistryDir(MozillaProductType product)
{
    if (product == MozillaProductType_Default)
        return ::rtl::OUString();

    return lcl_guessProfileRoot( product );
}

// -----------------------------------------------------------------------
::rtl::OUString getRegistryFileName(MozillaProductType product)
{
    if (product == MozillaProductType_Default)
        return ::rtl::OUString();

    return getRegistryDir(product) + ::rtl::OUString::createFromAscii(APP_REGISTRY_NAME);
}


