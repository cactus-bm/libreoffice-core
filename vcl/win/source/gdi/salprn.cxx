/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: salprn.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 14:06:39 $
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

#include <string.h>
#ifndef _SVWIN_H
#include <tools/svwin.h>
#endif

#ifndef _SV_WINCOMP_HXX
#include <wincomp.hxx>
#endif
#ifndef _SV_SALDATA_HXX
#include <saldata.hxx>
#endif
#ifndef _SV_SALINST_H
#include <salinst.h>
#endif
#ifndef _SV_SALGDI_H
#include <salgdi.h>
#endif
#ifndef _SV_SALFRAME_H
#include <salframe.h>
#endif
#ifndef _SV_SALPTYPE_HXX
#include <salptype.hxx>
#endif
#ifndef _SV_SALPRN_H
#include <salprn.h>
#endif

#ifndef _NEW_HXX
#include <tools/new.hxx>
#endif

#ifndef _SV_PRINT_H
#include <print.h>
#endif
#ifndef _SV_JOBSET_H
#include <jobset.h>
#endif

#include <tools/urlobj.hxx>
#ifndef  _COM_SUN_STAR_UI_DIALOGS_TEMPLATEDESCRIPTION_HPP_
#include <com/sun/star/ui/dialogs/TemplateDescription.hpp>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_EXECUTABLEDIALOGRESULTS_HPP_
#include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_XFILEPICKER_HPP_
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_XFILTERMANAGER_HPP_
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef  _COM_SUN_STAR_LANG_XINITIALIZATION_HPP_
#include <com/sun/star/lang/XInitialization.hpp>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#include <malloc.h>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::ui::dialogs;
using namespace rtl;

// =======================================================================

static char aImplWindows[] = "windows";
static char aImplDevices[] = "devices";
static char aImplDevice[]  = "device";

// =======================================================================

static ULONG ImplWinQueueStatusToSal( DWORD nWinStatus )
{
    ULONG nStatus = 0;
    if ( nWinStatus & PRINTER_STATUS_PAUSED )
        nStatus |= QUEUE_STATUS_PAUSED;
    if ( nWinStatus & PRINTER_STATUS_ERROR )
        nStatus |= QUEUE_STATUS_ERROR;
    if ( nWinStatus & PRINTER_STATUS_PENDING_DELETION )
        nStatus |= QUEUE_STATUS_PENDING_DELETION;
    if ( nWinStatus & PRINTER_STATUS_PAPER_JAM )
        nStatus |= QUEUE_STATUS_PAPER_JAM;
    if ( nWinStatus & PRINTER_STATUS_PAPER_OUT )
        nStatus |= QUEUE_STATUS_PAPER_OUT;
    if ( nWinStatus & PRINTER_STATUS_MANUAL_FEED )
        nStatus |= QUEUE_STATUS_MANUAL_FEED;
    if ( nWinStatus & PRINTER_STATUS_PAPER_PROBLEM )
        nStatus |= QUEUE_STATUS_PAPER_PROBLEM;
    if ( nWinStatus & PRINTER_STATUS_OFFLINE )
        nStatus |= QUEUE_STATUS_OFFLINE;
    if ( nWinStatus & PRINTER_STATUS_IO_ACTIVE )
        nStatus |= QUEUE_STATUS_IO_ACTIVE;
    if ( nWinStatus & PRINTER_STATUS_BUSY )
        nStatus |= QUEUE_STATUS_BUSY;
    if ( nWinStatus & PRINTER_STATUS_PRINTING )
        nStatus |= QUEUE_STATUS_PRINTING;
    if ( nWinStatus & PRINTER_STATUS_OUTPUT_BIN_FULL )
        nStatus |= QUEUE_STATUS_OUTPUT_BIN_FULL;
    if ( nWinStatus & PRINTER_STATUS_WAITING )
        nStatus |= QUEUE_STATUS_WAITING;
    if ( nWinStatus & PRINTER_STATUS_PROCESSING )
        nStatus |= QUEUE_STATUS_PROCESSING;
    if ( nWinStatus & PRINTER_STATUS_INITIALIZING )
        nStatus |= QUEUE_STATUS_INITIALIZING;
    if ( nWinStatus & PRINTER_STATUS_WARMING_UP )
        nStatus |= QUEUE_STATUS_WARMING_UP;
    if ( nWinStatus & PRINTER_STATUS_TONER_LOW )
        nStatus |= QUEUE_STATUS_TONER_LOW;
    if ( nWinStatus & PRINTER_STATUS_NO_TONER )
        nStatus |= QUEUE_STATUS_NO_TONER;
    if ( nWinStatus & PRINTER_STATUS_PAGE_PUNT )
        nStatus |= QUEUE_STATUS_PAGE_PUNT;
    if ( nWinStatus & PRINTER_STATUS_USER_INTERVENTION )
        nStatus |= QUEUE_STATUS_USER_INTERVENTION;
    if ( nWinStatus & PRINTER_STATUS_OUT_OF_MEMORY )
        nStatus |= QUEUE_STATUS_OUT_OF_MEMORY;
    if ( nWinStatus & PRINTER_STATUS_DOOR_OPEN )
        nStatus |= QUEUE_STATUS_DOOR_OPEN;
    if ( nWinStatus & PRINTER_STATUS_SERVER_UNKNOWN )
        nStatus |= QUEUE_STATUS_SERVER_UNKNOWN;
    if ( nWinStatus & PRINTER_STATUS_POWER_SAVE )
        nStatus |= QUEUE_STATUS_POWER_SAVE;
    if ( !nStatus && !(nWinStatus & PRINTER_STATUS_NOT_AVAILABLE) )
        nStatus |= QUEUE_STATUS_READY;
    return nStatus;
}

// -----------------------------------------------------------------------

void WinSalInstance::GetPrinterQueueInfo( ImplPrnQueueList* pList )
{
// !!! UNICODE - NT Optimierung !!!
    DWORD           i;
    DWORD           n;
    DWORD           nBytes = 0;
//    DWORD           nInfoRet;
    DWORD           nInfoPrn2;
    BOOL            bFound = FALSE;
    PRINTER_INFO_2* pWinInfo2 = NULL;
    PRINTER_INFO_2* pGetInfo2;
    EnumPrintersA( PRINTER_ENUM_LOCAL, NULL, 2, NULL, 0, &nBytes, &nInfoPrn2 );
    if ( nBytes )
    {
        pWinInfo2 = (PRINTER_INFO_2*) rtl_allocateMemory( nBytes );
        if ( EnumPrintersA( PRINTER_ENUM_LOCAL, NULL, 2, (LPBYTE)pWinInfo2, nBytes, &nBytes, &nInfoPrn2 ) )
        {
            pGetInfo2 = pWinInfo2;
            for ( i = 0; i < nInfoPrn2; i++ )
            {
                SalPrinterQueueInfo* pInfo = new SalPrinterQueueInfo;
                pInfo->maPrinterName = ImplSalGetUniString( pGetInfo2->pPrinterName );
                pInfo->maDriver      = ImplSalGetUniString( pGetInfo2->pDriverName );
                XubString aPortName;
                if ( pGetInfo2->pPortName )
                    aPortName = ImplSalGetUniString( pGetInfo2->pPortName );
                // pLocation can be 0 (the Windows docu doesn't describe this)
                if ( pGetInfo2->pLocation && strlen( pGetInfo2->pLocation ) )
                    pInfo->maLocation = ImplSalGetUniString( pGetInfo2->pLocation );
                else
                    pInfo->maLocation = aPortName;
                // pComment can be 0 (the Windows docu doesn't describe this)
                if ( pGetInfo2->pComment )
                    pInfo->maComment = ImplSalGetUniString( pGetInfo2->pComment );
                pInfo->mnStatus      = ImplWinQueueStatusToSal( pGetInfo2->Status );
                pInfo->mnJobs        = pGetInfo2->cJobs;
                pInfo->mpSysData     = new XubString( aPortName );
                pList->Add( pInfo );
                pGetInfo2++;
            }

            bFound = TRUE;
        }
    }

/* Siehe Kommentar unten !!!
    EnumPrinters( PRINTER_ENUM_NETWORK | PRINTER_ENUM_REMOTE, NULL, 1, NULL, 0, &nBytes, &nInfoRet );
    if ( nBytes )
    {
        PRINTER_INFO_1* pWinInfo1 = (PRINTER_INFO_1*)new BYTE[nBytes];
        if ( EnumPrinters( PRINTER_ENUM_NETWORK | PRINTER_ENUM_REMOTE, NULL, 1, (LPBYTE)pWinInfo1, nBytes, &nBytes, &nInfoRet ) )
        {
            PRINTER_INFO_1* pGetInfo1 = pWinInfo1;
            for ( i = 0; i < nInfoRet; i++ )
            {
                // Feststellen, ob Printer durch erste Abfrage schon gefunden
                // wurde
                BOOL bAdd = TRUE;
                if ( pWinInfo2 )
                {
                    pGetInfo2 = pWinInfo2;
                    for ( n = 0; n < nInfoPrn2; n++ )
                    {
                        if ( strcmp( pGetInfo1->pName, pGetInfo2->pPrinterName ) == 0 )
                        {
                            bAdd = FALSE;
                            break;
                        }
                        pGetInfo2++;
                    }
                }
                // Wenn neuer Printer, dann aufnehmen
                if ( bAdd )
                {
                    SalPrinterQueueInfo*    pInfo = new SalPrinterQueueInfo;
                    XubString               aPrnName( pGetInfo1->pName );
                    pInfo->maPrinterName = aPrnName;
                    pInfo->maDriver      = "winspool";
                    pInfo->maComment     = pGetInfo1->pComment;
                    pInfo->mnStatus      = 0;
                    pInfo->mnJobs        = QUEUE_JOBS_DONTKNOW;
                    pInfo->mpSysData     = new String();
                    pList->Add( pInfo );
                }
                pGetInfo1++;
            }

            bFound = TRUE;
        }

        delete pWinInfo1;
    }
*/

//    if ( bFound )
//        return;

// !!! UNICODE - NT Optimierung !!!
    // Drucker aus WIN.INI lesen
    UINT    nSize = 4096;
    char*   pBuf = new char[nSize];
    UINT    nRead = GetProfileStringA( aImplDevices, NULL, "", pBuf, nSize );
    while ( nRead >= nSize-2 )
    {
        nSize += 2048;
        delete []pBuf;
        pBuf = new char[nSize];
        nRead = GetProfileStringA( aImplDevices, NULL, "", pBuf, nSize );
    }

    // Druckernamen aus Buffer extrahieren und Liste aufbauen
    char* pName = pBuf;
    while ( *pName )
    {
        char*   pPortName;
        char*   pTmp;
        char    aPortBuf[256];
        GetProfileStringA( aImplDevices, pName, "", aPortBuf, sizeof( aPortBuf ) );

        pPortName = aPortBuf;

        // Namen anlegen
        xub_StrLen nNameLen = strlen( pName );
        XubString aName( ImplSalGetUniString( pName, nNameLen ) );

        // Treibernamen rausfischen
        pTmp = pPortName;
        while ( *pTmp != ',' )
            pTmp++;
        XubString aDriver( ImplSalGetUniString( pPortName, (USHORT)(pTmp-pPortName) ) );
        pPortName = pTmp;

        // Alle Portnamen raussuchen
        do
        {
            pPortName++;
            pTmp = pPortName;
            while ( *pTmp && (*pTmp != ',') )
                pTmp++;

            String aPortName( ImplSalGetUniString( pPortName, (USHORT)(pTmp-pPortName) ) );

            // Neuen Eintrag anlegen
            // !!! Da ich zu bloeb bin, die Netzwerk-Printer zur 5.0
            // !!! richtig zu integrieren, gehen wir zusaetzlich
            // !!! noch ueber das W16-Interface, da uns dort die
            // !!! Drucker noch einfach und schnell geliefert werden
            // !!! ohne das wir jetzt zu grossen Aufwand treiben muessen.
            // !!! Somit sollten wir dann jedenfalls nicht schlechter sein
            // !!! als in einer 4.0 SP2.
            // Feststellen, ob Printer durch erste Abfrage schon gefunden
            // wurde
            BOOL bAdd = TRUE;
            if ( pWinInfo2 )
            {
                pGetInfo2 = pWinInfo2;
                for ( n = 0; n < nInfoPrn2; n++ )
                {
                    if ( aName.EqualsIgnoreCaseAscii( pGetInfo2->pPrinterName ) )
                    {
                        bAdd = FALSE;
                        break;
                    }
                    pGetInfo2++;
                }
            }
            // Wenn neuer Printer, dann aufnehmen
            if ( bAdd )
            {
                SalPrinterQueueInfo* pInfo = new SalPrinterQueueInfo;
                pInfo->maPrinterName = aName;
                pInfo->maDriver      = aDriver;
                pInfo->maLocation    = aPortName;
                pInfo->mnStatus      = 0;
                pInfo->mnJobs        = QUEUE_JOBS_DONTKNOW;
                pInfo->mpSysData     = new XubString( aPortName );
                pList->Add( pInfo );
            }
        }
        while ( *pTmp == ',' );

        pName += nNameLen + 1;
    }

    delete []pBuf;
    rtl_freeMemory( pWinInfo2 );
}

// -----------------------------------------------------------------------

void WinSalInstance::GetPrinterQueueState( SalPrinterQueueInfo* pInfo )
{
// !!! UNICODE - NT Optimierung !!!
    DWORD               nBytes = 0;
    DWORD               nInfoRet;
    PRINTER_INFO_2*     pWinInfo2;
    EnumPrintersA( PRINTER_ENUM_LOCAL, NULL, 2, NULL, 0, &nBytes, &nInfoRet );
    if ( nBytes )
    {
        pWinInfo2 = (PRINTER_INFO_2*) rtl_allocateMemory( nBytes );
        if ( EnumPrintersA( PRINTER_ENUM_LOCAL, NULL, 2, (LPBYTE)pWinInfo2, nBytes, &nBytes, &nInfoRet ) )
        {
            PRINTER_INFO_2* pGetInfo2 = pWinInfo2;
            for ( DWORD i = 0; i < nInfoRet; i++ )
            {
                if ( pInfo->maPrinterName.EqualsAscii( pGetInfo2->pPrinterName ) &&
                     pInfo->maDriver.EqualsAscii( pGetInfo2->pDriverName ) )
                {
                    if ( pGetInfo2->pLocation && strlen( pGetInfo2->pLocation ) )
                        pInfo->maLocation = ImplSalGetUniString( pGetInfo2->pLocation );
                    else
                        pInfo->maLocation = ImplSalGetUniString( pGetInfo2->pPortName );
                    pInfo->mnStatus = ImplWinQueueStatusToSal( pGetInfo2->Status );
                    pInfo->mnJobs   = pGetInfo2->cJobs;
                    break;
                }

                pGetInfo2++;
            }
        }

        rtl_freeMemory( pWinInfo2 );
    }
}

// -----------------------------------------------------------------------

void WinSalInstance::DeletePrinterQueueInfo( SalPrinterQueueInfo* pInfo )
{
    delete (String*)(pInfo->mpSysData);
    delete pInfo;
}

// -----------------------------------------------------------------------

// !!! UNICODE - NT Optimierung !!!
XubString WinSalInstance::GetDefaultPrinter()
{
    // Default-Printer-String aus win.ini holen
    char szBuffer[256];
    GetProfileStringA( aImplWindows, aImplDevice, "", szBuffer, sizeof( szBuffer ) );
    if ( szBuffer[0] )
    {
        // Printername suchen
        char* pBuf = szBuffer;
        char* pTmp = pBuf;
        while ( *pTmp && (*pTmp != ',') )
            pTmp++;
        return ImplSalGetUniString( pBuf, (xub_StrLen)(pTmp-pBuf) );
    }
    else
        return XubString();
}

// =======================================================================

static DWORD ImplDeviceCaps( WinSalInfoPrinter* pPrinter, WORD nCaps,
                             LPTSTR pOutput, const ImplJobSetup* pSetupData )
{
    DEVMODE* pDevMode;
    if ( !pSetupData || !pSetupData->mpDriverData )
        pDevMode = NULL;
    else
        pDevMode = SAL_DEVMODE( pSetupData );

// !!! UNICODE - NT Optimierung !!!
    return DeviceCapabilitiesA( ImplSalGetWinAnsiString( pPrinter->maDeviceName, TRUE ).GetBuffer(),
                                ImplSalGetWinAnsiString( pPrinter->maPortName, TRUE ).GetBuffer(),
                                nCaps, (LPSTR)pOutput, pDevMode );
}

// -----------------------------------------------------------------------

static BOOL ImplTestSalJobSetup( WinSalInfoPrinter* pPrinter,
                                 ImplJobSetup* pSetupData, BOOL bDelete )
{
    if ( pSetupData && pSetupData->mpDriverData )
    {
        // signature and size must fit to avoid using
        // JobSetups from a wrong system

        // initialize versions from jobsetup
        // those will be overwritten with driver's version
        DEVMODE *pDevMode = SAL_DEVMODE( pSetupData );
        LONG dmSpecVersion = pDevMode->dmSpecVersion;
        LONG dmDriverVersion = pDevMode->dmDriverVersion;

        if( pPrinter )
        {
            // just too many driver crashes in that area -> check the dmSpecVersion and dmDriverVersion fields always !!!
            // this prevents using the jobsetup between different Windows versions (eg from XP to 9x) but we
            // can avoid potential driver crashes as their jobsetups are often not compatible
            // #110800#, #111151#, #112381#, #i16580#, #i14173# and perhaps #112375#
            HANDLE hPrn;
            if ( !OpenPrinterA( (LPSTR)ImplSalGetWinAnsiString( pPrinter->maDeviceName, TRUE ).GetBuffer(), &hPrn, NULL ) )
                return FALSE;

            long nSysJobSize = DocumentPropertiesA( 0, hPrn,
                                                    (LPSTR)ImplSalGetWinAnsiString( pPrinter->maDeviceName, TRUE ).GetBuffer(),
                                                    NULL, NULL, 0 );
            if( nSysJobSize < 0 )
            {
                ClosePrinter( hPrn );
                return FALSE;
            }
            DEVMODE *pBuffer = (DEVMODE*) _alloca( nSysJobSize );
            DWORD nRet = DocumentPropertiesA( 0, hPrn,
                                            (LPSTR)ImplSalGetWinAnsiString( pPrinter->maDeviceName, TRUE ).GetBuffer(),
                                            pBuffer, NULL, DM_OUT_BUFFER );
            if( nRet < 0 )
            {
                ClosePrinter( hPrn );
                return FALSE;
            }

            // the spec version differs between the windows platforms, ie 98,NT,2000/XP
            // this allows us to throw away printer settings from other platforms that might crash a buggy driver
            // we check the driver version as well
            dmSpecVersion = pBuffer->dmSpecVersion;
            dmDriverVersion = pBuffer->dmDriverVersion;

            ClosePrinter( hPrn );
        }

        if ( (pSetupData->mnSystem == JOBSETUP_SYSTEM_WINDOWS) &&
             (pPrinter->maDriverName == pSetupData->maDriver) &&
             (pSetupData->mnDriverDataLen > sizeof( SalDriverData )) &&
             (dmSpecVersion == pDevMode->dmSpecVersion) &&
             (dmDriverVersion == pDevMode->dmDriverVersion) &&
             (((SalDriverData*)(pSetupData->mpDriverData))->mnSysSignature == SAL_DRIVERDATA_SYSSIGN) )
            return TRUE;
        else if ( bDelete )
        {
            rtl_freeMemory( pSetupData->mpDriverData );
            pSetupData->mpDriverData = NULL;
            pSetupData->mnDriverDataLen = 0;
        }
    }

    return FALSE;
}

// -----------------------------------------------------------------------

static BOOL ImplUpdateSalJobSetup( WinSalInfoPrinter* pPrinter, ImplJobSetup* pSetupData,
                                   BOOL bIn, WinSalFrame* pVisibleDlgParent )
{
    HANDLE hPrn;
// !!! UNICODE - NT Optimierung !!!
    if ( !OpenPrinterA( (LPSTR)ImplSalGetWinAnsiString( pPrinter->maDeviceName, TRUE ).GetBuffer(), &hPrn, NULL ) )
        return FALSE;

    LONG            nRet;
    LONG            nSysJobSize;
    HWND            hWnd = 0;
    DWORD           nMode = DM_OUT_BUFFER;
    ULONG           nDriverDataLen = 0;
    SalDriverData*  pOutBuffer = NULL;
    DEVMODE*        pInDevBuffer = NULL;
    DEVMODE*        pOutDevBuffer = NULL;

// !!! UNICODE - NT Optimierung !!!
    nSysJobSize = DocumentPropertiesA( hWnd, hPrn,
                                       (LPSTR)ImplSalGetWinAnsiString( pPrinter->maDeviceName, TRUE ).GetBuffer(),
                                       NULL, NULL, 0 );
    if ( nSysJobSize < 0 )
    {
        ClosePrinter( hPrn );
        return FALSE;
    }

    // Outputbuffer anlegen
    nDriverDataLen              = sizeof(SalDriverData)+nSysJobSize-1;
    pOutBuffer                  = (SalDriverData*)rtl_allocateMemory( nDriverDataLen );
    memset( pOutBuffer, 0, nDriverDataLen );
    pOutDevBuffer               = (LPDEVMODE)(pOutBuffer->maDriverData);
    pOutBuffer->mnSysSignature  = SAL_DRIVERDATA_SYSSIGN;
    pOutBuffer->mnVersion       = SAL_DRIVERDATA_VERSION;
    pOutBuffer->mnDriverOffset  = (USHORT)(((SalDriverData*)NULL)->maDriverData);

    // Testen, ob wir einen geeigneten Inputbuffer haben
    if ( bIn && ImplTestSalJobSetup( pPrinter, pSetupData, FALSE ) )
    {
        pInDevBuffer = SAL_DEVMODE( pSetupData );
        nMode |= DM_IN_BUFFER;
    }

    // Testen, ob Dialog angezeigt werden soll
    if ( pVisibleDlgParent )
    {
        hWnd = pVisibleDlgParent->mhWnd;
        nMode |= DM_IN_PROMPT;
    }

// !!! UNICODE - NT Optimierung !!!
    // Release mutex, in the other case we don't get paints and so on
    ULONG nMutexCount=0;
    if ( pVisibleDlgParent )
        nMutexCount = ImplSalReleaseYieldMutex();

    nRet = DocumentPropertiesA( hWnd, hPrn,
                                (LPSTR)ImplSalGetWinAnsiString( pPrinter->maDeviceName, TRUE ).GetBuffer(),
                                pOutDevBuffer, pInDevBuffer, nMode );
    if ( pVisibleDlgParent )
        ImplSalAcquireYieldMutex( nMutexCount );
    ClosePrinter( hPrn );

    if ( (nRet < 0) || (pVisibleDlgParent && (nRet == IDCANCEL)) )
    {
        rtl_freeMemory( pOutBuffer );
        return FALSE;
    }

    // String-Buffer am Ende immer mit 0 initialisieren, damit
    // die JobSetups nach Moeglichkeit bei memcmp immer
    // identisch sind
    if ( pOutDevBuffer->dmSize >= 32 )
    {
        USHORT nLen = strlen( (const char*)pOutDevBuffer->dmDeviceName );
        if ( nLen < sizeof( pOutDevBuffer->dmDeviceName ) )
            memset( pOutDevBuffer->dmDeviceName+nLen, 0, sizeof( pOutDevBuffer->dmDeviceName )-nLen );
    }
    if ( pOutDevBuffer->dmSize >= 102 )
    {
        USHORT nLen = strlen( (const char*)pOutDevBuffer->dmFormName );
        if ( nLen < sizeof( pOutDevBuffer->dmFormName ) )
            memset( pOutDevBuffer->dmFormName+nLen, 0, sizeof( pOutDevBuffer->dmFormName )-nLen );
    }

    // Daten updaten
    if ( pSetupData->mpDriverData )
        rtl_freeMemory( pSetupData->mpDriverData );
    pSetupData->mnDriverDataLen = nDriverDataLen;
    pSetupData->mpDriverData    = (BYTE*)pOutBuffer;
    pSetupData->mnSystem        = JOBSETUP_SYSTEM_WINDOWS;

    return TRUE;
}

// -----------------------------------------------------------------------

static void ImplDevModeToJobSetup( WinSalInfoPrinter* pPrinter, ImplJobSetup* pSetupData, ULONG nFlags )
{
    if ( !pSetupData || !pSetupData->mpDriverData )
        return;

    DEVMODE* pDevMode = SAL_DEVMODE( pSetupData );

    // Orientation
    if ( nFlags & SAL_JOBSET_ORIENTATION )
    {
        if ( pDevMode->dmOrientation == DMORIENT_PORTRAIT )
            pSetupData->meOrientation = ORIENTATION_PORTRAIT;
        else if ( pDevMode->dmOrientation == DMORIENT_LANDSCAPE )
            pSetupData->meOrientation = ORIENTATION_LANDSCAPE;
    }

    // PaperBin
    if ( nFlags & SAL_JOBSET_PAPERBIN )
    {
        ULONG nCount = ImplDeviceCaps( pPrinter, DC_BINS, NULL, pSetupData );

        if ( nCount && (nCount != ((ULONG)-1)) )
        {
            WORD* pBins = new WORD[nCount];
            memset( (BYTE*)pBins, 0, nCount*sizeof(WORD) );
            ImplDeviceCaps( pPrinter, DC_BINS, (LPTSTR)pBins, pSetupData );
            pSetupData->mnPaperBin = 0;

            // search the right bin and assign index to mnPaperBin
            for( ULONG i = 0; i < nCount; i++ )
            {
                if( pDevMode->dmDefaultSource == pBins[ i ] )
                {
                    pSetupData->mnPaperBin = (USHORT)i;
                    break;
                }
            }

            delete[] pBins;
        }
    }

    // PaperSize
    if ( nFlags & SAL_JOBSET_PAPERSIZE )
    {
        pSetupData->mnPaperWidth  = pDevMode->dmPaperWidth*10;
        pSetupData->mnPaperHeight = pDevMode->dmPaperLength*10;
        switch( pDevMode->dmPaperSize )
        {
            case( DMPAPER_A3 ):
                pSetupData->mePaperFormat = PAPER_A3;
                break;
            case( DMPAPER_A4 ):
                pSetupData->mePaperFormat = PAPER_A4;
                break;
            case( DMPAPER_A5 ):
                pSetupData->mePaperFormat = PAPER_A5;
                break;
            case( DMPAPER_B4 ):
                pSetupData->mePaperFormat = PAPER_B4;
                break;
            case( DMPAPER_B5 ):
                pSetupData->mePaperFormat = PAPER_B5;
                break;
            case( DMPAPER_LETTER ):
                pSetupData->mePaperFormat = PAPER_LETTER;
                break;
            case( DMPAPER_LEGAL ):
                pSetupData->mePaperFormat = PAPER_LEGAL;
                break;
            case( DMPAPER_TABLOID ):
                pSetupData->mePaperFormat = PAPER_TABLOID;
                break;
            default:
                pSetupData->mePaperFormat = PAPER_USER;
                break;
        }
    }
}

// -----------------------------------------------------------------------

static BOOL ImplPaperSizeEqual( short nPaperWidth1, short nPaperHeight1,
                                short nPaperWidth2, short nPaperHeight2 )
{
    const short PAPER_SLOPPY = 1; // 0.1 mm accuracy

    return ( (Abs( nPaperWidth1-nPaperWidth2 ) <= PAPER_SLOPPY) &&
             (Abs( nPaperHeight1-nPaperHeight2 ) <= PAPER_SLOPPY) );
}

// -----------------------------------------------------------------------

static void ImplJobSetupToDevMode( WinSalInfoPrinter* pPrinter, ImplJobSetup* pSetupData, ULONG nFlags )
{
    if ( !pSetupData || !pSetupData->mpDriverData )
        return;

    DEVMODE* pDevMode = SAL_DEVMODE( pSetupData );

    // Orientation
    if ( nFlags & SAL_JOBSET_ORIENTATION )
    {
        pDevMode->dmFields |= DM_ORIENTATION;
        if ( pSetupData->meOrientation == ORIENTATION_PORTRAIT )
            pDevMode->dmOrientation = DMORIENT_PORTRAIT;
        else
            pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
    }

    // PaperBin
    if ( nFlags & SAL_JOBSET_PAPERBIN )
    {
        ULONG nCount = ImplDeviceCaps( pPrinter, DC_BINS, NULL, pSetupData );

        if ( nCount && (nCount != ((ULONG)-1)) )
        {
            WORD* pBins = new WORD[nCount];
            memset( pBins, 0, nCount*sizeof(WORD) );
            ImplDeviceCaps( pPrinter, DC_BINS, (LPTSTR)pBins, pSetupData );
            pDevMode->dmFields |= DM_DEFAULTSOURCE;
            pDevMode->dmDefaultSource = pBins[ pSetupData->mnPaperBin ];
            delete[] pBins;
        }
    }

    // PaperSize
    if ( nFlags & SAL_JOBSET_PAPERSIZE )
    {
        pDevMode->dmFields      |= DM_PAPERSIZE;
        pDevMode->dmPaperWidth   = 0;
        pDevMode->dmPaperLength  = 0;

        switch( pSetupData->mePaperFormat )
        {
            case( PAPER_A3 ):
                pDevMode->dmPaperSize = DMPAPER_A3;
                break;
            case( PAPER_A4 ):
                pDevMode->dmPaperSize = DMPAPER_A4;
                break;
            case( PAPER_A5 ):
                pDevMode->dmPaperSize = DMPAPER_A5;
                break;
            case( PAPER_B4 ):
                pDevMode->dmPaperSize = DMPAPER_B4;
                break;
            case( PAPER_B5 ):
                pDevMode->dmPaperSize = DMPAPER_B5;
                break;
            case( PAPER_LETTER ):
                pDevMode->dmPaperSize = DMPAPER_LETTER;
                break;
            case( PAPER_LEGAL ):
                pDevMode->dmPaperSize = DMPAPER_LEGAL;
                break;
            case( PAPER_TABLOID ):
                pDevMode->dmPaperSize = DMPAPER_TABLOID;
                break;
            default:
            {
                short   nPaper = 0;
                ULONG   nPaperCount = ImplDeviceCaps( pPrinter, DC_PAPERS, NULL, pSetupData );
                WORD*   pPapers = NULL;
                ULONG   nPaperSizeCount = ImplDeviceCaps( pPrinter, DC_PAPERSIZE, NULL, pSetupData );
                POINT*  pPaperSizes = NULL;
                DWORD   nLandscapeAngle = ImplDeviceCaps( pPrinter, DC_ORIENTATION, NULL, pSetupData );
                if ( nPaperCount && (nPaperCount != ((ULONG)-1)) )
                {
                    pPapers = new WORD[nPaperCount];
                    memset( pPapers, 0, nPaperCount*sizeof(WORD) );
                    ImplDeviceCaps( pPrinter, DC_PAPERS, (LPTSTR)pPapers, pSetupData );
                }
                if ( nPaperSizeCount && (nPaperSizeCount != ((ULONG)-1)) )
                {
                    pPaperSizes = new POINT[nPaperSizeCount];
                    memset( pPaperSizes, 0, nPaperSizeCount*sizeof(POINT) );
                    ImplDeviceCaps( pPrinter, DC_PAPERSIZE, (LPTSTR)pPaperSizes, pSetupData );
                }
                if ( (nPaperSizeCount == nPaperCount) && pPapers && pPaperSizes )
                {
                    // Alle Papierformate vergleichen und ein passendes
                    // raussuchen
                    for ( ULONG i = 0; i < nPaperCount; i++ )
                    {
                        if ( ImplPaperSizeEqual( (short)(pSetupData->mnPaperWidth/10),
                                                (short)(pSetupData->mnPaperHeight/10),
                                                (short)pPaperSizes[i].x,
                                                (short)pPaperSizes[i].y ) )
                        {
                            nPaper = pPapers[i];
                            break;
                        }
                    }

                    // If the printer supports landscape orientation, check paper sizes again
                    // with landscape orientation. This is necessary as a printer driver provides
                    // all paper sizes with portrait orientation only!!
                    if ( !nPaper && nLandscapeAngle != 0 )
                    {
                        for ( ULONG i = 0; i < nPaperCount; i++ )
                        {
                            if ( ImplPaperSizeEqual( (short)(pSetupData->mnPaperWidth/10),
                                                    (short)(pSetupData->mnPaperHeight/10),
                                                    (short)pPaperSizes[i].y,
                                                    (short)pPaperSizes[i].x ) )
                            {
                                nPaper = pPapers[i];
                                break;
                            }
                        }
                    }

                    if ( nPaper )
                        pDevMode->dmPaperSize = nPaper;
                }

                if ( !nPaper )
                {
                    pDevMode->dmFields     |= DM_PAPERLENGTH | DM_PAPERWIDTH;
                    pDevMode->dmPaperSize   = DMPAPER_USER;
                    pDevMode->dmPaperWidth  = (short)(pSetupData->mnPaperWidth/10);
                    pDevMode->dmPaperLength = (short)(pSetupData->mnPaperHeight/10);
                }

                if ( pPapers )
                    delete [] pPapers;
                if ( pPaperSizes )
                    delete [] pPaperSizes;

                break;
            }
        }
    }
}

// -----------------------------------------------------------------------

static HDC ImplCreateSalPrnIC( WinSalInfoPrinter* pPrinter, ImplJobSetup* pSetupData )
{
    LPDEVMODE pDevMode;
    if ( pSetupData && pSetupData->mpDriverData )
        pDevMode = SAL_DEVMODE( pSetupData );
    else
        pDevMode = NULL;
// !!! UNICODE - NT Optimierung !!!
    // #95347 some buggy drivers (eg, OKI) write to those buffers in CreateIC, although declared const - so provide some space
    ByteString aDriver ( ImplSalGetWinAnsiString( pPrinter->maDriverName, TRUE ) );
    ByteString aDevice ( ImplSalGetWinAnsiString( pPrinter->maDeviceName, TRUE ) );
    int n = aDriver.Len() > aDevice.Len() ? aDriver.Len() : aDevice.Len();
    n += 2048;
    char *lpszDriverName = new char[n];
    char *lpszDeviceName = new char[n];
    strncpy( lpszDriverName, aDriver.GetBuffer(), n );
    strncpy( lpszDeviceName, aDevice.GetBuffer(), n );
    HDC hDC = CreateICA( lpszDriverName,
                         lpszDeviceName,
                         0,
                         (LPDEVMODE)pDevMode );
    delete [] lpszDriverName;
    delete [] lpszDeviceName;
    return hDC;
}

// -----------------------------------------------------------------------

static WinSalGraphics* ImplCreateSalPrnGraphics( HDC hDC )
{
    WinSalGraphics* pGraphics = new WinSalGraphics;
    pGraphics->SetLayout( 0 );
    pGraphics->mhDC     = hDC;
    pGraphics->mhWnd    = 0;
    pGraphics->mbPrinter = TRUE;
    pGraphics->mbVirDev = FALSE;
    pGraphics->mbWindow = FALSE;
    pGraphics->mbScreen = FALSE;
    ImplSalInitGraphics( pGraphics );
    return pGraphics;
}

// -----------------------------------------------------------------------

static BOOL ImplUpdateSalPrnIC( WinSalInfoPrinter* pPrinter, ImplJobSetup* pSetupData )
{
    HDC hNewDC = ImplCreateSalPrnIC( pPrinter, pSetupData );
    if ( !hNewDC )
        return FALSE;

    if ( pPrinter->mpGraphics )
    {
        ImplSalDeInitGraphics( pPrinter->mpGraphics );
        DeleteDC( pPrinter->mpGraphics->mhDC );
        delete pPrinter->mpGraphics;
    }

    WinSalGraphics* pGraphics = ImplCreateSalPrnGraphics( hNewDC );
    pPrinter->mhDC      = hNewDC;
    pPrinter->mpGraphics    = pGraphics;

    return TRUE;
}

// =======================================================================

SalInfoPrinter* WinSalInstance::CreateInfoPrinter( SalPrinterQueueInfo* pQueueInfo,
                                                   ImplJobSetup* pSetupData )
{
    WinSalInfoPrinter* pPrinter = new WinSalInfoPrinter;
    pPrinter->maDriverName  = pQueueInfo->maDriver;
    pPrinter->maDeviceName  = pQueueInfo->maPrinterName;
    pPrinter->maPortName        = *(String*)(pQueueInfo->mpSysData);

    // Testen, ob Setupdaten zum Drucker gehoeren (erst aufrufen, nachdem
    // die Member gesetzt sind, da diese in dieser Routine abgefragt werden)
    ImplTestSalJobSetup( pPrinter, pSetupData, TRUE );

    HDC hDC = ImplCreateSalPrnIC( pPrinter, pSetupData );
    if ( !hDC )
    {
        delete pPrinter;
        return NULL;
    }

    WinSalGraphics* pGraphics = ImplCreateSalPrnGraphics( hDC );
    pPrinter->mhDC      = hDC;
    pPrinter->mpGraphics    = pGraphics;
    if ( !pSetupData->mpDriverData )
        ImplUpdateSalJobSetup( pPrinter, pSetupData, FALSE, NULL );
    ImplDevModeToJobSetup( pPrinter, pSetupData, SAL_JOBSET_ALL );
    pSetupData->mnSystem = JOBSETUP_SYSTEM_WINDOWS;

    return pPrinter;
}

// -----------------------------------------------------------------------

void WinSalInstance::DestroyInfoPrinter( SalInfoPrinter* pPrinter )
{
    delete pPrinter;
}

// =======================================================================

WinSalInfoPrinter::WinSalInfoPrinter()
{
    mhDC            = 0;
    mpGraphics  = NULL;
    mbGraphics  = FALSE;
    m_bPapersInit               = FALSE;
}

// -----------------------------------------------------------------------

WinSalInfoPrinter::~WinSalInfoPrinter()
{
    if ( mpGraphics )
    {
        ImplSalDeInitGraphics( mpGraphics );
        DeleteDC( mpGraphics->mhDC );
        delete mpGraphics;
    }
}

// -----------------------------------------------------------------------

void WinSalInfoPrinter::InitPaperFormats( const ImplJobSetup* pSetupData )
{
    m_aPaperFormats.clear();

    ULONG   nCount = ImplDeviceCaps( this, DC_PAPERSIZE, NULL, pSetupData );
    POINT*  pPaperSizes = NULL;
    char* pNamesBuffer = NULL;
    if ( nCount && (nCount != ((ULONG)-1)) )
    {
        pPaperSizes = new POINT[nCount];
        memset( pPaperSizes, 0, nCount*sizeof(POINT) );
        ImplDeviceCaps( this, DC_PAPERSIZE, (LPTSTR)pPaperSizes, pSetupData );

        pNamesBuffer = new char[nCount*64];
        ImplDeviceCaps( this, DC_PAPERNAMES, pNamesBuffer, pSetupData );
    }

    for ( ULONG i = 0; i < nCount; i++ )
    {
        vcl::PaperInfo aInfo;
        aInfo.m_nPaperWidth  = (pPaperSizes[i].x + 5) / 10;
        aInfo.m_nPaperHeight = (pPaperSizes[i].y + 5) / 10;
        pNamesBuffer[(i+1)*64-1] = '\0';    // make very long names zero terminated
        aInfo.m_aPaperName = ImplSalGetUniString( (const char*)(pNamesBuffer + (i*64)) );
        m_aPaperFormats.push_back( aInfo );
    }

    delete [] pNamesBuffer;
    delete [] pPaperSizes;
    m_bPapersInit = true;
}

// -----------------------------------------------------------------------

DuplexMode WinSalInfoPrinter::GetDuplexMode( const ImplJobSetup* pSetupData )
{
    DuplexMode nRet = DUPLEX_UNKNOWN;
    DEVMODE* pDevMode;
    if ( !pSetupData || !pSetupData->mpDriverData )
        pDevMode = NULL;
    else
        pDevMode = SAL_DEVMODE( pSetupData );

    if ( pDevMode && (pDevMode->dmFields & DM_DUPLEX ))
    {
        if ( pDevMode->dmDuplex == DMDUP_SIMPLEX )
            nRet = DUPLEX_OFF;
        else
            nRet = DUPLEX_ON;
    }
    return nRet;
}

// -----------------------------------------------------------------------

int WinSalInfoPrinter::GetLandscapeAngle( const ImplJobSetup* pSetupData )
{
    int nRet = ImplDeviceCaps( this, DC_ORIENTATION, NULL, pSetupData );

    if( nRet != -1 )
        return nRet * 10;
    else
        return 900; // guess
}

// -----------------------------------------------------------------------

SalGraphics* WinSalInfoPrinter::GetGraphics()
{
    if ( mbGraphics )
        return NULL;

    if ( mpGraphics )
        mbGraphics = TRUE;

    return mpGraphics;
}

// -----------------------------------------------------------------------

void WinSalInfoPrinter::ReleaseGraphics( SalGraphics* )
{
    mbGraphics = FALSE;
}

// -----------------------------------------------------------------------

BOOL WinSalInfoPrinter::Setup( SalFrame* pFrame, ImplJobSetup* pSetupData )
{
    if ( ImplUpdateSalJobSetup( this, pSetupData, TRUE, static_cast<WinSalFrame*>(pFrame) ) )
    {
        ImplDevModeToJobSetup( this, pSetupData, SAL_JOBSET_ALL );
        return ImplUpdateSalPrnIC( this, pSetupData );
    }

    return FALSE;
}

// -----------------------------------------------------------------------

BOOL WinSalInfoPrinter::SetPrinterData( ImplJobSetup* pSetupData )
{
    if ( !ImplTestSalJobSetup( this, pSetupData, FALSE ) )
        return FALSE;
    return ImplUpdateSalPrnIC( this, pSetupData );
}

// -----------------------------------------------------------------------

BOOL WinSalInfoPrinter::SetData( ULONG nFlags, ImplJobSetup* pSetupData )
{
    ImplJobSetupToDevMode( this, pSetupData, nFlags );
    if ( ImplUpdateSalJobSetup( this, pSetupData, TRUE, NULL ) )
    {
        ImplDevModeToJobSetup( this, pSetupData, nFlags );
        return ImplUpdateSalPrnIC( this, pSetupData );
    }

    return FALSE;
}

// -----------------------------------------------------------------------

ULONG WinSalInfoPrinter::GetPaperBinCount( const ImplJobSetup* pSetupData )
{
    DWORD nRet = ImplDeviceCaps( this, DC_BINS, NULL, pSetupData );
    if ( nRet && (nRet != ((ULONG)-1)) )
        return nRet;
    else
        return 0;
}

// -----------------------------------------------------------------------

XubString WinSalInfoPrinter::GetPaperBinName( const ImplJobSetup* pSetupData, ULONG nPaperBin )
{
// !!! UNICODE - NT Optimierung !!!
    XubString aPaperBinName;

    DWORD nBins = ImplDeviceCaps( this, DC_BINNAMES, NULL, pSetupData );
    if ( (nPaperBin < nBins) && (nBins != ((ULONG)-1)) )
    {
        char* pBuffer = new char[nBins*24];
        DWORD nRet = ImplDeviceCaps( this, DC_BINNAMES, pBuffer, pSetupData );
        if ( nRet && (nRet != ((ULONG)-1)) )
            aPaperBinName = ImplSalGetUniString( (const char*)(pBuffer + (nPaperBin*24)) );
        delete [] pBuffer;
    }

    return aPaperBinName;
}

// -----------------------------------------------------------------------

ULONG WinSalInfoPrinter::GetCapabilities( const ImplJobSetup* pSetupData, USHORT nType )
{
    DWORD nRet;

    switch ( nType )
    {
        case PRINTER_CAPABILITIES_SUPPORTDIALOG:
            return TRUE;
        case PRINTER_CAPABILITIES_COPIES:
            nRet = ImplDeviceCaps( this, DC_COPIES, NULL, pSetupData );
            if ( nRet && (nRet != ((ULONG)-1)) )
                return nRet;
            return 0;
        case PRINTER_CAPABILITIES_COLLATECOPIES:
            if ( aSalShlData.mbW40 )
            {
                nRet = ImplDeviceCaps( this, DC_COLLATE, NULL, pSetupData );
                if ( nRet && (nRet != ((ULONG)-1)) )
                {
                    nRet = ImplDeviceCaps( this, DC_COPIES, NULL, pSetupData );
                    if ( nRet && (nRet != ((ULONG)-1)) )
                         return nRet;
                }
            }
            return 0;

        case PRINTER_CAPABILITIES_SETORIENTATION:
            nRet = ImplDeviceCaps( this, DC_ORIENTATION, NULL, pSetupData );
            if ( nRet && (nRet != ((ULONG)-1)) )
                return TRUE;
            return FALSE;

        case PRINTER_CAPABILITIES_SETPAPERBIN:
            nRet = ImplDeviceCaps( this, DC_BINS, NULL, pSetupData );
            if ( nRet && (nRet != ((ULONG)-1)) )
                return TRUE;
            return FALSE;

        case PRINTER_CAPABILITIES_SETPAPERSIZE:
        case PRINTER_CAPABILITIES_SETPAPER:
            nRet = ImplDeviceCaps( this, DC_PAPERS, NULL, pSetupData );
            if ( nRet && (nRet != ((ULONG)-1)) )
                return TRUE;
            return FALSE;
    }

    return 0;
}

// -----------------------------------------------------------------------

void WinSalInfoPrinter::GetPageInfo( const ImplJobSetup*,
                                  long& rOutWidth, long& rOutHeight,
                                  long& rPageOffX, long& rPageOffY,
                                  long& rPageWidth, long& rPageHeight )
{
    HDC hDC = mhDC;

    rOutWidth   = GetDeviceCaps( hDC, HORZRES );
    rOutHeight  = GetDeviceCaps( hDC, VERTRES );

    rPageOffX   = GetDeviceCaps( hDC, PHYSICALOFFSETX );
    rPageOffY   = GetDeviceCaps( hDC, PHYSICALOFFSETY );
    rPageWidth  = GetDeviceCaps( hDC, PHYSICALWIDTH );
    rPageHeight = GetDeviceCaps( hDC, PHYSICALHEIGHT );
}

// =======================================================================

SalPrinter* WinSalInstance::CreatePrinter( SalInfoPrinter* pInfoPrinter )
{
    WinSalPrinter* pPrinter = new WinSalPrinter;
    pPrinter->mpInfoPrinter = static_cast<WinSalInfoPrinter*>(pInfoPrinter);
    return pPrinter;
}

// -----------------------------------------------------------------------

void WinSalInstance::DestroyPrinter( SalPrinter* pPrinter )
{
    delete pPrinter;
}

// =======================================================================

WIN_BOOL CALLBACK SalPrintAbortProc( HDC hPrnDC, int /* nError */ )
{
    SalData*    pSalData = GetSalData();
    WinSalPrinter* pPrinter;
    BOOL        bWhile = TRUE;
    int         i = 0;

    do
    {
        // Messages verarbeiten
        MSG aMsg;
        if ( ImplPeekMessage( &aMsg, 0, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &aMsg );
            ImplDispatchMessage( &aMsg );
            i++;
            if ( i > 15 )
                bWhile = FALSE;
        }
        else
            bWhile = FALSE;

        pPrinter = pSalData->mpFirstPrinter;
        while ( pPrinter )
        {
            if( pPrinter->mhDC == hPrnDC )
                break;

            pPrinter = pPrinter->mpNextPrinter;
        }

        if ( !pPrinter || pPrinter->mbAbort )
            return FALSE;
    }
    while ( bWhile );

    return TRUE;
}

// -----------------------------------------------------------------------

static LPDEVMODE ImplSalSetCopies( LPDEVMODE pDevMode, ULONG nCopies, BOOL bCollate )
{
    LPDEVMODE pNewDevMode = pDevMode;
    if ( pDevMode && (nCopies > 1) )
    {
        if ( nCopies > 32765 )
            nCopies = 32765;
        ULONG nDevSize = pDevMode->dmSize+pDevMode->dmDriverExtra;
        pNewDevMode = (LPDEVMODE)rtl_allocateMemory( nDevSize );
        memcpy( pNewDevMode, pDevMode, nDevSize );
        pDevMode = pNewDevMode;
        pDevMode->dmFields |= DM_COPIES;
        pDevMode->dmCopies  = (short)(USHORT)nCopies;
        if ( aSalShlData.mbW40 )
        {
            pDevMode->dmFields |= DM_COLLATE;
            if ( bCollate )
                pDevMode->dmCollate = DMCOLLATE_TRUE;
            else
                pDevMode->dmCollate = DMCOLLATE_FALSE;
        }
    }

    return pNewDevMode;
}

// -----------------------------------------------------------------------

WinSalPrinter::WinSalPrinter()
{
    SalData* pSalData = GetSalData();

    mhDC            = 0;
    mpGraphics  = NULL;
    mbAbort     = FALSE;
    mnCopies        = 0;
    mbCollate   = FALSE;

    // insert frame in framelist
    mpNextPrinter = pSalData->mpFirstPrinter;
    pSalData->mpFirstPrinter = this;
}

// -----------------------------------------------------------------------

WinSalPrinter::~WinSalPrinter()
{
    SalData* pSalData = GetSalData();

    // DC freigeben, wenn er noch durch ein AbortJob existiert
    HDC hDC = mhDC;
    if ( hDC )
    {
        if ( mpGraphics )
        {
            ImplSalDeInitGraphics( mpGraphics );
            delete mpGraphics;
        }

        DeleteDC( hDC );
    }

    // remove printer from printerlist
    if ( this == pSalData->mpFirstPrinter )
        pSalData->mpFirstPrinter = mpNextPrinter;
    else
    {
        WinSalPrinter* pTempPrinter = pSalData->mpFirstPrinter;

        while( pTempPrinter->mpNextPrinter != this )
            pTempPrinter = pTempPrinter->mpNextPrinter;

        pTempPrinter->mpNextPrinter = mpNextPrinter;
    }
}

// -----------------------------------------------------------------------

BOOL WinSalPrinter::StartJob( const XubString* pFileName,
                           const XubString& rJobName,
                           const XubString&,
                           ULONG nCopies, BOOL bCollate,
                           ImplJobSetup* pSetupData )
{
    mnError     = 0;
    mbAbort     = FALSE;
    mnCopies        = nCopies;
    mbCollate   = bCollate;

    LPDEVMODE   pOrgDevMode = NULL;
    LPDEVMODE   pDevMode;
    BOOL        bOwnDevMode = FALSE;
    if ( pSetupData && pSetupData->mpDriverData )
    {
        pOrgDevMode = SAL_DEVMODE( pSetupData );
        pDevMode = ImplSalSetCopies( pOrgDevMode, nCopies, bCollate );
    }
    else
        pDevMode = NULL;

// !!! UNICODE - NT Optimierung !!!
    // #95347 some buggy drivers (eg, OKI) write to those buffers in CreateDC, although declared const - so provide some space
    ByteString aDriver ( ImplSalGetWinAnsiString( mpInfoPrinter->maDriverName, TRUE ) );
    ByteString aDevice ( ImplSalGetWinAnsiString( mpInfoPrinter->maDeviceName, TRUE ) );
    ByteString aPort   ( ImplSalGetWinAnsiString( mpInfoPrinter->maPortName, TRUE ) );
    int n = aDriver.Len() > aDevice.Len() ? aDriver.Len() : aDevice.Len();
    n += 2048;
    char *lpszDriverName = new char[n];
    char *lpszDeviceName = new char[n];
    strncpy( lpszDriverName, aDriver.GetBuffer(), n );
    strncpy( lpszDeviceName, aDevice.GetBuffer(), n );
    HDC hDC = CreateDCA( lpszDriverName,
                         lpszDeviceName,
                         NULL,
                         (LPDEVMODE)pDevMode );

    delete [] lpszDriverName;
    delete [] lpszDeviceName;

    if ( pDevMode != pOrgDevMode )
        rtl_freeMemory( pDevMode );

    if ( !hDC )
    {
        mnError = SAL_PRINTER_ERROR_GENERALERROR;
        return FALSE;
    }

    if ( SetAbortProc( hDC, SalPrintAbortProc ) <= 0 )
    {
        mnError = SAL_PRINTER_ERROR_GENERALERROR;
        return FALSE;
    }

    mnError = 0;
    mbAbort = FALSE;

// !!! UNICODE - NT Optimierung !!!
    // Both strings must be exist, if StartJob() is called
    ByteString aJobName( ImplSalGetWinAnsiString( rJobName, TRUE ) );
    ByteString aFileName;

    DOCINFOA aInfo;
    memset( &aInfo, 0, sizeof( DOCINFO ) );
    aInfo.cbSize = sizeof( aInfo );
    aInfo.lpszDocName = (LPCSTR)aJobName.GetBuffer();
    if ( pFileName )
    {
        if ( pFileName->Len() )
        {
            aFileName = ImplSalGetWinAnsiString( *pFileName, TRUE );
            aInfo.lpszOutput = (LPCSTR)aFileName.GetBuffer();
        }
        else
            aInfo.lpszOutput = "FILE:";
    }
    else
        aInfo.lpszOutput = NULL;

    // Wegen Telocom Balloon Fax-Treiber, der uns unsere Messages
    // ansonsten oefters schickt, versuchen wir vorher alle
    // zu verarbeiten und dann eine Dummy-Message reinstellen
    BOOL bWhile = TRUE;
    int  i = 0;
    do
    {
        // Messages verarbeiten
        MSG aMsg;
        if ( ImplPeekMessage( &aMsg, 0, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &aMsg );
            ImplDispatchMessage( &aMsg );
            i++;
            if ( i > 15 )
                bWhile = FALSE;
        }
        else
            bWhile = FALSE;
    }
    while ( bWhile );
    ImplPostMessage( GetSalData()->mpFirstInstance->mhComWnd, SAL_MSG_DUMMY, 0, 0 );

    // make sure mhDC is set before the printer driver may call our abortproc
    mhDC = hDC;

    // bring up a file choser if printing to file port but no file name given
    OString aOutFileName;
    if( aPort.EqualsIgnoreCaseAscii( "FILE:" ) && !aInfo.lpszOutput )
    {

        Reference< XMultiServiceFactory > xFactory( ::comphelper::getProcessServiceFactory() );
        if( xFactory.is() )
        {
            Reference< XFilePicker > xFilePicker( xFactory->createInstance(
                OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.ui.dialogs.FilePicker" ) ) ),
                UNO_QUERY );
            DBG_ASSERT( xFilePicker.is(), "could not get FilePicker service" );

            Reference< XInitialization > xInit( xFilePicker, UNO_QUERY );
            Reference< XFilterManager > xFilterMgr( xFilePicker, UNO_QUERY );
            if( xInit.is() && xFilePicker.is() && xFilterMgr.is() )
            {
                Sequence< Any > aServiceType( 1 );
                aServiceType[0] <<= TemplateDescription::FILESAVE_SIMPLE;
                xInit->initialize( aServiceType );
                if( xFilePicker->execute() == ExecutableDialogResults::OK )
                {
                    Sequence< OUString > aPathSeq( xFilePicker->getFiles() );
                    INetURLObject aObj( aPathSeq[0] );
                    // we're using ansi calls (StartDocA) so convert the string
                    aOutFileName = OUStringToOString( aObj.PathToFileName(), osl_getThreadTextEncoding() );
                    aInfo.lpszOutput = aOutFileName.getStr();
                }
                else
                {
                    mnError = SAL_PRINTER_ERROR_ABORT;
                    return FALSE;
                }
            }
        }
    }

    // start Job
    int nRet = ::StartDocA( hDC, &aInfo );
    if ( nRet <= 0 )
    {
        long nError = GetLastError();
        if ( (nRet == SP_USERABORT) || (nRet == SP_APPABORT) || (nError == ERROR_PRINT_CANCELLED) || (nError == ERROR_CANCELLED) )
            mnError = SAL_PRINTER_ERROR_ABORT;
        else
            mnError = SAL_PRINTER_ERROR_GENERALERROR;
        return FALSE;
    }

    return TRUE;
}

// -----------------------------------------------------------------------

BOOL WinSalPrinter::EndJob()
{
    DWORD err = 0;
    HDC hDC = mhDC;
    if ( hDC )
    {
        if ( mpGraphics )
        {
            ImplSalDeInitGraphics( mpGraphics );
            delete mpGraphics;
            mpGraphics = NULL;
        }

        if( ::EndDoc( hDC ) <= 0 )
            err = GetLastError();
        DeleteDC( hDC );
    }

    return TRUE;
}

// -----------------------------------------------------------------------

BOOL WinSalPrinter::AbortJob()
{
    mbAbort = TRUE;

    // Abort asyncron ausloesen
    HDC hDC = mhDC;
    if ( hDC )
    {
        SalData* pSalData = GetSalData();
        ImplPostMessage( pSalData->mpFirstInstance->mhComWnd,
                         SAL_MSG_PRINTABORTJOB, (WPARAM)hDC, 0 );
    }

    return TRUE;
}

// -----------------------------------------------------------------------

void ImplSalPrinterAbortJobAsync( HDC hPrnDC )
{
    SalData*    pSalData = GetSalData();
    WinSalPrinter* pPrinter = pSalData->mpFirstPrinter;

    // Feststellen, ob Printer noch existiert
    while ( pPrinter )
    {
        if ( pPrinter->mhDC == hPrnDC )
            break;

        pPrinter = pPrinter->mpNextPrinter;
    }

    // Wenn Printer noch existiert, dann den Job abbrechen
    if ( pPrinter )
    {
        HDC hDC = pPrinter->mhDC;
        if ( hDC )
        {
            if ( pPrinter->mpGraphics )
            {
                ImplSalDeInitGraphics( pPrinter->mpGraphics );
                delete pPrinter->mpGraphics;
                pPrinter->mpGraphics = NULL;
            }

            ::AbortDoc( hDC );
            DeleteDC( hDC );
        }
    }
}

// -----------------------------------------------------------------------

SalGraphics* WinSalPrinter::StartPage( ImplJobSetup* pSetupData, BOOL bNewJobData )
{
    HDC hDC = mhDC;
    if ( pSetupData && pSetupData->mpDriverData && bNewJobData )
    {
        LPDEVMODE   pOrgDevMode;
        LPDEVMODE   pDevMode;
        pOrgDevMode = SAL_DEVMODE( pSetupData );
        pDevMode = ImplSalSetCopies( pOrgDevMode, mnCopies, mbCollate );
        ResetDC( hDC, pDevMode );
        if ( pDevMode != pOrgDevMode )
            rtl_freeMemory( pDevMode );
    }
    int nRet = ::StartPage( hDC );
    if ( nRet <= 0 )
    {
        DWORD err = GetLastError();
        mnError = SAL_PRINTER_ERROR_GENERALERROR;
        return NULL;
    }

    // Hack, damit alte PS-Treiber Leerseiten nicht wegoptimieren
    HPEN    hTempPen = SelectPen( hDC, GetStockPen( NULL_PEN ) );
    HBRUSH  hTempBrush = SelectBrush( hDC, GetStockBrush( NULL_BRUSH ) );
    WIN_Rectangle( hDC, -8000, -8000, -7999, -7999 );
    SelectPen( hDC, hTempPen );
    SelectBrush( hDC, hTempBrush );

    WinSalGraphics* pGraphics = new WinSalGraphics;
    pGraphics->mhDC     = hDC;
    pGraphics->mhWnd    = 0;
    pGraphics->mbPrinter = TRUE;
    pGraphics->mbVirDev = FALSE;
    pGraphics->mbWindow = FALSE;
    pGraphics->mbScreen = FALSE;
    ImplSalInitGraphics( pGraphics );
    mpGraphics = pGraphics;
    return pGraphics;
}

// -----------------------------------------------------------------------

BOOL WinSalPrinter::EndPage()
{
    HDC hDC = mhDC;
    if ( hDC && mpGraphics )
    {
        ImplSalDeInitGraphics( mpGraphics );
        delete mpGraphics;
        mpGraphics = NULL;
    }
    int nRet = ::EndPage( hDC );
    if ( nRet > 0 )
        return TRUE;
    else
    {
        DWORD err = GetLastError();
        mnError = SAL_PRINTER_ERROR_GENERALERROR;
        return FALSE;
    }
}

// -----------------------------------------------------------------------

ULONG WinSalPrinter::GetErrorCode()
{
    return mnError;
}
