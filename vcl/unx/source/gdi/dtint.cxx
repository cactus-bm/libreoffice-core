/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dtint.cxx,v $
 *
 *  $Revision: 1.26 $
 *
 *  last change: $Author: rt $ $Date: 2006-03-07 09:42:35 $
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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>

#include <salunx.h>
#include <X11/Xatom.h>

#ifdef USE_CDE
#include <cdeint.hxx>
#endif
#include <dtint.hxx>
#ifdef MACOSX
#include <macosxint.hxx>
#endif
#include <saldisp.hxx>
#include <saldata.hxx>
#include <wmadaptor.hxx>

#include <svapp.hxx>
#include <dtsetenum.hxx>

#include <osl/file.h>
#include <osl/process.h>
#include <osl/security.h>

#include <set>
#include <stdio.h>

// NETBSD has no RTLD_GLOBAL
#ifndef RTLD_GLOBAL
#define DLOPEN_MODE (RTLD_LAZY)
#else
#define DLOPEN_MODE (RTLD_GLOBAL | RTLD_LAZY)
#endif


using namespace rtl;
using namespace vcl_sal;

BOOL bSymbolLoadFailed = FALSE;

DtIntegratorList DtIntegrator::aIntegratorList;
String DtIntegrator::aHomeDir;

DtIntegrator::DtIntegrator() :
        meType( DtGeneric ),
        mnRefCount( 0 ),
        mnSystemLookCommandProcess( -1 )
{
    mpSalDisplay = GetSalData()->GetDisplay();
    mpDisplay = mpSalDisplay->GetDisplay();
    aIntegratorList.Insert( this, LIST_APPEND );
    OUString aDir;
    oslSecurity aCur = osl_getCurrentSecurity();
    if( aCur )
    {
        osl_getHomeDir( aCur, &aDir.pData );
        osl_freeSecurityHandle( aCur );
        OUString aSysDir;
        osl_getSystemPathFromFileURL( aDir.pData, &aSysDir.pData );
        aHomeDir = aSysDir;
    }
}

DtIntegrator::~DtIntegrator()
{
}

DtIntegrator* DtIntegrator::CreateDtIntegrator()
{
    SalDisplay* pSalDisplay = GetSalData()->GetDisplay();
    Display* pDisplay = pSalDisplay->GetDisplay();

    for( unsigned int i = 0; i < aIntegratorList.Count(); i++ )
    {
        DtIntegrator* pIntegrator = aIntegratorList.GetObject( i );
        if( pIntegrator->mpDisplay == pDisplay )
            return pIntegrator;
    }
#ifdef MACOSX
    return new MACOSXIntegrator();
#endif
    /*
     *  #i22061# override desktop detection
     *  if environment variable OOO_FORCE_DESKTOP is set
     *  to one of "cde" "kde" "gnome" then autodetection
     *  is overridden.
     */
    static const char* pOverride = getenv( "OOO_FORCE_DESKTOP" );
    if( pOverride && *pOverride )
    {
        OString aOver( pOverride );

#if USE_CDE
        if( aOver.equalsIgnoreAsciiCase( "cde" ) )
            return new CDEIntegrator();
#endif
        if( aOver.equalsIgnoreAsciiCase( "none" ) )
            return new DtIntegrator();
    }

#ifdef USE_CDE
    void* pLibrary = NULL;

    // check dt type
    // CDE
    Atom nDtAtom = XInternAtom( pDisplay, "_DT_WM_READY", True );
    if( nDtAtom && ( pLibrary = dlopen( "/usr/dt/lib/libDtSvc.so", DLOPEN_MODE ) ) )
    {
        dlclose( pLibrary );
        return new CDEIntegrator();
    }
#endif

    // default: generic implementation
    return new DtIntegrator();
}

void DtIntegrator::GetSystemLook( AllSettings& rSettings )
{
    // #i48001# set a default blink rate
    StyleSettings aStyleSettings = rSettings.GetStyleSettings();
    aStyleSettings.SetCursorBlinkTime( 500 );
    rSettings.SetStyleSettings( aStyleSettings );
}
