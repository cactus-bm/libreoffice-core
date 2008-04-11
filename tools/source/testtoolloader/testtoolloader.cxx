/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: testtoolloader.cxx,v $
 * $Revision: 1.9 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_tools.hxx"

#include "tools/testtoolloader.hxx"
#include <osl/module.h>
#include <rtl/logfile.hxx>
#include <vos/process.hxx>
#include "tools/solar.h"
#include "tools/string.hxx"
#include "tools/debug.hxx"

using namespace rtl;

namespace tools
{
    typedef void ( *pfunc_CreateRemoteControl)();
    typedef void ( *pfunc_DestroyRemoteControl)();

static oslModule    aTestToolModule = 0;


sal_uInt32 GetCommandLineParamCount()
{
    NAMESPACE_VOS( OStartupInfo ) aStartInfo;
    return aStartInfo.getCommandArgCount();
}

String GetCommandLineParam( sal_uInt32 nParam )
{
    NAMESPACE_VOS( OStartupInfo ) aStartInfo;
    ::rtl::OUString aParam;
    NAMESPACE_VOS( OStartupInfo )::TStartupError eError = aStartInfo.getCommandArg( nParam, aParam );
    if ( eError == NAMESPACE_VOS( OStartupInfo )::E_None )
        return String( aParam );
    else
    {
        DBG_ERROR( "Unable to get CommandLineParam" );
        return String();
    }
}

extern "C" { static void SAL_CALL thisModule() {} }

void InitTestToolLib()
{
    RTL_LOGFILE_CONTEXT( aLog, "desktop (cd100003) ::InitTestToolLib" );

    sal_uInt32 i;
    // are we to be automated at all?
    bool bAutomate = false;

    for ( i = 0 ; i < GetCommandLineParamCount() ; i++ )
    {
        if ( GetCommandLineParam( i ).EqualsIgnoreCaseAscii("/enableautomation")
            || GetCommandLineParam( i ).EqualsIgnoreCaseAscii("-enableautomation"))
        {
            bAutomate = true;
            break;
        }
    }

    if ( !bAutomate )
        return;


    OUString    aFuncName( RTL_CONSTASCII_USTRINGPARAM( "CreateRemoteControl" ));

    aTestToolModule = osl_loadModuleRelative(
        &thisModule,
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SVLIBRARY("sts"))).pData,
        SAL_LOADMODULE_DEFAULT );
    if ( aTestToolModule )
    {
        oslGenericFunction pInitFunc = osl_getFunctionSymbol(
            aTestToolModule, aFuncName.pData );
        if ( pInitFunc )
            (reinterpret_cast< pfunc_CreateRemoteControl >(pInitFunc))();
        else
        {
            DBG_ERROR1( "Unable to get Symbol 'CreateRemoteControl' from library %s while loading testtool support.", SVLIBRARY( "sts" ) );
        }
    }
    else
    {
        DBG_ERROR1( "Unable to access library %s while loading testtool support.", SVLIBRARY( "sts" ) );
    }
}

void DeInitTestToolLib()
{
    if ( aTestToolModule )
    {
        OUString    aFuncName( RTL_CONSTASCII_USTRINGPARAM( "DestroyRemoteControl" ));

        oslGenericFunction pDeInitFunc = osl_getFunctionSymbol(
            aTestToolModule, aFuncName.pData );
        if ( pDeInitFunc )
            (reinterpret_cast< pfunc_DestroyRemoteControl >(pDeInitFunc))();

        osl_unloadModule( aTestToolModule );
    }
}

} // namespace tools
