/*************************************************************************
 *
 *  $RCSfile: salinst.cxx,v $
 *
 *  $Revision: 1.32 $
 *
 *  last change: $Author: vg $ $Date: 2004-01-06 12:43:10 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *   - GNU Lesser General Public License Version 2.1
 *   - Sun Industry Standards Source License Version 1.1
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

#include <stdio.h>

#ifndef _SV_SALDATA_HXX
#include <saldata.hxx>
#endif
#ifndef _SV_SALINST_HXX
#include <salinst.hxx>
#endif
#ifndef _SV_SALFRAME_HXX
#include <salframe.hxx>
#endif
#ifndef _SV_SALOBJ_HXX
#include <salobj.hxx>
#endif
#ifndef _SV_SALSYS_HXX
#include <salsys.hxx>
#endif
#ifndef _SV_SALVD_HXX
#include <salvd.hxx>
#endif
#ifndef _SV_DIALOG_HXX
#include <dialog.hxx>
#endif
#ifndef _SV_VCLAPPLICATION_H
#include <VCLApplication.h>
#endif
#ifndef _SV_VCLAUTORELEASEPOOL_H
#include <VCLAutoreleasePool.h>
#endif
#ifndef _FSYS_HXX
#include <tools/fsys.hxx>
#endif

static VCLAUTORELEASEPOOL hMainAutoreleasePool = NULL;

// =======================================================================

void SalAbort( const XubString& rErrorText )
{
    if( !rErrorText.Len() )
        fprintf( stderr, "Application Error " );
    else
        fprintf( stderr, "%s ",
            ByteString( rErrorText, gsl_getSystemTextEncoding() ).GetBuffer() );
    abort();
}

// -----------------------------------------------------------------------

void InitSalData()
{
    SalData *pSalData = new SalData;
    memset( pSalData, 0, sizeof( SalData ) );
    SetSalData( pSalData );
}

// -----------------------------------------------------------------------

void DeInitSalData()
{
    SalData *pSalData = GetSalData();
    delete pSalData;
    SetSalData( NULL );
}

// -----------------------------------------------------------------------

#ifdef QUARTZ
extern "C" {
#include <crt_externs.h>
}
#endif

void InitSalMain()
{
    // [ed] 5/14/02 We need to use _NSGetEnviron() here and can't use the straight
    // environ.  This is due to two-level-namespaces not enjoying having funky
    // global hack symbols lying around.
#ifdef QUARTZ
    char **environ;
    char *stackNULL=NULL;
    if(_NSGetEnviron())
        environ=*_NSGetEnviron();
    else
        environ=&stackNULL;
#else
    extern char **environ;
#endif

    char **pEnviron;

    // Get full executable path. We cna't use __progname as that only holds
    // the name of the executable and not the path. The full executable path
    // is listed after the first NULL in *environ.
    pEnviron = environ;
    while ( *pEnviron++ )
        ;

    // Need to include the absolute path for this executable in the PATH
    // and STAR_RESOURCEPATH environment variables so that the resource manager
    // can find resource files and in the DYLD_LIBRARY_PATH environment
    // variable so that the dynamic library loader can find shared libraries
    ByteString aPath( getenv( "PATH" ) );
    ByteString aResPath( getenv( "STAR_RESOURCEPATH" ) );
    ByteString aLibPath( getenv( "DYLD_LIBRARY_PATH" ) );
    ByteString aCmdPath( *pEnviron );
    ByteString aTmpPath;
    // Get absolute path of command's directory
    if ( aCmdPath.Len() ) {
        DirEntry aCmdDirEntry( aCmdPath );
        aCmdDirEntry.ToAbs();
        aCmdPath = ByteString( aCmdDirEntry.GetPath().GetFull(), RTL_TEXTENCODING_ASCII_US );
    }
    // Assign to PATH environment variable
    if ( aCmdPath.Len() )
    {
        aTmpPath = ByteString( "PATH=" );
        aTmpPath += aCmdPath;
        if ( aPath.Len() )
            aTmpPath += ByteString( DirEntry::GetSearchDelimiter(), RTL_TEXTENCODING_ASCII_US );
        aTmpPath += aPath;
        putenv( aTmpPath.GetBuffer() );
    }
    // Assign to STAR_RESOURCEPATH environment variable
    if ( aCmdPath.Len() )
    {
        aTmpPath = ByteString( "STAR_RESOURCEPATH=" );
        aTmpPath += aCmdPath;
        if ( aResPath.Len() )
            aTmpPath += ByteString( DirEntry::GetSearchDelimiter(), RTL_TEXTENCODING_ASCII_US );
        aTmpPath += aResPath;
        putenv( aTmpPath.GetBuffer() );
    }
    // Assign to DYLD_LIBRARY_PATH environment variable
    if ( aCmdPath.Len() )
    {
        aTmpPath = ByteString( "DYLD_LIBRARY_PATH=" );
        aTmpPath += aCmdPath;
        if ( aLibPath.Len() )
            aTmpPath += ByteString( DirEntry::GetSearchDelimiter(), RTL_TEXTENCODING_ASCII_US );
        aTmpPath += aLibPath;
        putenv( aTmpPath.GetBuffer() );
    }

    // Setup up autorelease pool for Objective-C objects
    hMainAutoreleasePool = VCLAutoreleasePool_Init();

    // Initialize application's connection to the window server
    VCLApplication_SharedApplication();
}

// -----------------------------------------------------------------------

void DeInitSalMain()
{
    // Release autorelease pool
    VCLAutoreleasePool_Release( hMainAutoreleasePool );

}

// -----------------------------------------------------------------------

void SetFilterCallback( void* pCallback, void* pInst )
{
    SalData *pSalData = GetSalData();
    pSalData->mpFirstInstance->maInstData.mpFilterCallback = pCallback;
    pSalData->mpFirstInstance->maInstData.mpFilterInst = pInst;
}

// =======================================================================

SalYieldMutex::SalYieldMutex()
{
    mnCount  = 0;
    mnThreadId  = 0;
}

void SalYieldMutex::acquire()
{
    OMutex::acquire();
    mnThreadId = NAMESPACE_VOS(OThread)::getCurrentIdentifier();
    mnCount++;
}

void SalYieldMutex::release()
{
    if ( mnThreadId == NAMESPACE_VOS(OThread)::getCurrentIdentifier() )
    {
        if ( mnCount == 1 )
            mnThreadId = 0;
        mnCount--;
    }
    OMutex::release();
}

sal_Bool SalYieldMutex::tryToAcquire()
{
    if ( OMutex::tryToAcquire() )
    {
        mnThreadId = NAMESPACE_VOS(OThread)::getCurrentIdentifier();
        mnCount++;
        return True;
    }
    else
        return False;
}

// =======================================================================

SalInstance* CreateSalInstance()
{
    SalData* pSalData = GetSalData();
    SalInstance* pInst = new SalInstance;

    // init instance (only one instance in this version !!!)
    pSalData->mpFirstInstance = pInst;

    return pInst;
}

// -----------------------------------------------------------------------

void DestroySalInstance( SalInstance* pInst )
{
    delete pInst;
}

// -----------------------------------------------------------------------

SalInstance::SalInstance()
{
    maInstData.mpFilterCallback = NULL;
    maInstData.mpFilterInst = NULL;
    maInstData.mpSalYieldMutex = new SalYieldMutex;
    maInstData.mpSalYieldMutex->acquire();
}

// -----------------------------------------------------------------------

SalInstance::~SalInstance()
{
    maInstData.mpSalYieldMutex->release();
    delete maInstData.mpSalYieldMutex;
}

// -----------------------------------------------------------------------

vos::IMutex* SalInstance::GetYieldMutex()
{
    return maInstData.mpSalYieldMutex;
}

// -----------------------------------------------------------------------

ULONG SalInstance::ReleaseYieldMutex()
{
    SalYieldMutex* pYieldMutex = maInstData.mpSalYieldMutex;
    if ( pYieldMutex->GetThreadId() ==
         NAMESPACE_VOS(OThread)::getCurrentIdentifier() )
    {
        ULONG nCount = pYieldMutex->GetAcquireCount();
        ULONG n = nCount;
        while ( n )
        {
            pYieldMutex->release();
            n--;
        }

        return nCount;
    }
    else
        return 0;
}

// -----------------------------------------------------------------------

void SalInstance::AcquireYieldMutex( ULONG nCount )
{
    SalYieldMutex* pYieldMutex = maInstData.mpSalYieldMutex;
    while ( nCount )
    {
        pYieldMutex->acquire();
        nCount--;
    }
}

// -----------------------------------------------------------------------

void SalInstance::Yield( BOOL bWait )
{
    ULONG nCount = 0;

    // Release all locks so that we don't deadlock when we pull pending
    // events from the event queue
    nCount = ReleaseYieldMutex();

    // Pull pending events from the event queue and dispatch them.
    VCLApplication_Run( bWait );

    // Reset all locks
    AcquireYieldMutex( nCount );
}

// -----------------------------------------------------------------------

BOOL SalInstance::AnyInput( USHORT nType )
{
    return FALSE;
}

// -----------------------------------------------------------------------

SalFrame* SalInstance::CreateChildFrame( SystemParentData* pSystemParentData, ULONG nSalFrameStyle )
{
    return NULL;
}

// -----------------------------------------------------------------------

SalFrame* SalInstance::CreateFrame( SalFrame* pParent, ULONG nSalFrameStyle )
{
    SalFrame *pFrame = new SalFrame;

    pFrame->maFrameData.mpParent = pParent;

    // Create the native window
    pFrame->maFrameData.mhWnd = VCLWindow_New( nSalFrameStyle, NULL,
        pFrame, &(pFrame->maFrameData) );

    return pFrame;
}

// -----------------------------------------------------------------------

void SalInstance::DestroyFrame( SalFrame* pFrame )
{
    delete pFrame;
}

// -----------------------------------------------------------------------

SalObject* SalInstance::CreateObject( SalFrame* pParent )
{
    SalObject *pObject = NULL;

    if ( pParent )
    {
        pObject = new SalObject;
        pObject->maObjectData.mpFrame = pParent;
    }

    return pObject;
}

// -----------------------------------------------------------------------

void SalInstance::DestroyObject( SalObject* pObject )
{
    delete ( pObject );
}

// -----------------------------------------------------------------------

SalVirtualDevice* SalInstance::CreateVirtualDevice( SalGraphics* pGraphics,
    long nDX, long nDY, USHORT nBitCount )
{
    SalVirtualDevice *pVirDev = new SalVirtualDevice;

    // Cache values for when SalVirtualDevice::GetGraphics() is invoked
    pVirDev->maVirDevData.mnBitCount = nBitCount;
    pVirDev->maVirDevData.mnWidth = nDX;
    pVirDev->maVirDevData.mnHeight = nDY;

    return pVirDev;
}

// -----------------------------------------------------------------------

void SalInstance::DestroyVirtualDevice( SalVirtualDevice* pDevice )
{
    delete pDevice;
}

// -----------------------------------------------------------------------

SalPrinter* SalInstance::CreatePrinter( SalInfoPrinter* pInfoPrinter )
{
    return NULL;
}

// -----------------------------------------------------------------------

void SalInstance::DestroyPrinter( SalPrinter* pPrinter )
{
}

// -----------------------------------------------------------------------

void SalInstance::GetPrinterQueueInfo( ImplPrnQueueList* pList )
{
}

// -----------------------------------------------------------------------

void SalInstance::GetPrinterQueueState( SalPrinterQueueInfo* pInfo )
{
}

// -----------------------------------------------------------------------

void SalInstance::DeletePrinterQueueInfo( SalPrinterQueueInfo* pInfo )
{
}

// -----------------------------------------------------------------------

XubString SalInstance::GetDefaultPrinter()
{
    return XubString();
}

// -----------------------------------------------------------------------

SalInfoPrinter* SalInstance::CreateInfoPrinter( SalPrinterQueueInfo* pQueueInfo,
                                                ImplJobSetup* pSetupData )
{
    return NULL;
}

// -----------------------------------------------------------------------

void SalInstance::DestroyInfoPrinter( SalInfoPrinter* pPrinter )
{
}

// -----------------------------------------------------------------------

SalSystem* SalInstance::CreateSystem()
{
    return new SalSystem();
}

// -----------------------------------------------------------------------

void SalInstance::DestroySystem( SalSystem* pSystem )
{
    delete pSystem;
}

// -----------------------------------------------------------------------

void SalInstance::SetEventCallback( void* pInstance, bool(*pCallback)(void*,void*,int) )
{
}

// -----------------------------------------------------------------------

void SalInstance::SetErrorEventCallback( void* pInstance, bool(*pCallback)(void*,void*,int) )
{
}

// -----------------------------------------------------------------------

void* SalInstance::GetConnectionIdentifier( ConnectionIdentifierType& rReturnedType, int& rReturnedBytes )
{
    rReturnedBytes  = 1;
    rReturnedType   = AsciiCString;
    return "";
}
