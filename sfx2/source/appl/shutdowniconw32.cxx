/*************************************************************************
 *
 *  $RCSfile: shutdowniconw32.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: ssa $ $Date: 2001-06-15 08:40:19 $
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

#ifdef WNT

//
// the systray icon is only available on windows
//

#include <shutdownicon.hxx>
#include <app.hrc>
//#include <windows.h>
#include <shlobj.h>
#include <objidl.h>
#include <stdio.h>
#include <io.h>
#include <osl/thread.h>
#include <osl/file.hxx>

#include <systools/win32/user9x.h> // initializiation is performed below !
#include <systools/win32/shell9x.h> // ""



//
// note: this code is copyied from SAL/systools
//  it initializes the functionpointers for the unicodewrappers
//

#define SYSTOOLS_DLL_NAME   "w9xucwrp.dll"

typedef BOOL ( WINAPI * InsertMenuItemW_Proc_T ) (
    HMENU hMenu,            // handle to menu
    UINT uItem,             // identifier or position
    BOOL fByPosition,       // meaning of uItem
    LPCMENUITEMINFOW lpmii  // menu item information
);
InsertMenuItemW_Proc_T lpfnInsertMenuItemW = NULL;

typedef BOOL ( WINAPI * SHGetPathFromIDListW_Proc_T ) (
    LPCITEMIDLIST pidl, LPWSTR pszPath );
SHGetPathFromIDListW_Proc_T lpfnSHGetPathFromIDListW = NULL;

static void WINAPI User9xInit( )
{
    if ( lpfnInsertMenuItemW && lpfnSHGetPathFromIDListW )
        //nothing to do
        return;

    HMODULE         hModule;
    OSVERSIONINFO   OSVerInfo;

    OSVerInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &OSVerInfo );

    // if windows 95/98
    if ( VER_PLATFORM_WIN32_WINDOWS == OSVerInfo.dwPlatformId )
    {
        hModule = LoadLibraryA( SYSTOOLS_DLL_NAME );
        if ( NULL != hModule )
        {
            lpfnInsertMenuItemW = ( InsertMenuItemW_Proc_T )GetProcAddress(
                hModule, "InsertMenuItemW_9x" );
            lpfnSHGetPathFromIDListW = (SHGetPathFromIDListW_Proc_T)GetProcAddress(
                hModule, "SHGetPathFromIDListW_9x" );
        }
    }
    else
    {
        hModule = LoadLibraryA( "user32.dll" );
        if ( NULL != hModule )
        {
            lpfnInsertMenuItemW = ( InsertMenuItemW_Proc_T )GetProcAddress(
                hModule, "InsertMenuItemW" );
        }
        hModule = LoadLibraryA( "Shell32.dll" );
        if ( NULL != hModule )
        {
            lpfnSHGetPathFromIDListW = (SHGetPathFromIDListW_Proc_T)GetProcAddress(
                                            hModule, "SHGetPathFromIDListW" );
        }
    }
}


using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::osl;


#define LISTENER_WINDOWCLASS    "SO Listener Class"
#define LISTENER_WINDOWNAME     "SO Listener Window"

#define ID_QUICKSTART               1
#define IDM_EXIT                    2
#define IDM_OPEN                    3
#define IDM_WRITER                  4
#define IDM_CALC                    5
#define IDM_IMPRESS                 6
#define IDM_DRAW                    7
#define IDM_TEMPLATE                8
#define IDM_INSTALL                 9
#define IDM_UNINSTALL               10


#define WRITER_URL      "private:factory/swriter"
#define CALC_URL        "private:factory/scalc"
#define IMPRESS_URL     "private:factory/simpress"
#define DRAW_URL        "private:factory/sdraw"
#define MATH_URL        "private:factory/smath"

#define ICON_SO_DEFAULT                 1
#define ICON_TEXT_DOCUMENT              2
#define ICON_TEXT_TEMPLATE              3
#define ICON_SPREADSHEET_DOCUMENT       4
#define ICON_SPREADSHEET_TEMPLATE       5
#define ICON_DRAWING_DOCUMENT           6
#define ICON_DRAWING_TEMPLATE           7
#define ICON_PRESENTATION_DOCUMENT      8
#define ICON_PRESENTATION_TEMPLATE      9
#define ICON_PRESENTATION_COMPRESSED    10
#define ICON_GLOBAL_DOCUMENT            11
#define ICON_HTML_DOCUMENT              12
#define ICON_CHART_DOCUMENT             13
#define ICON_DATABASE_DOCUMENT          14
#define ICON_MATH_DOCUMENT              15
#define ICON_TEMPLATE                   16
#define ICON_MACROLIBRARY               17
#define ICON_SETUP                      500

#define SFX_TASKBAR_NOTIFICATION    WM_USER+1

static HWND aListenerWindow = NULL;

static void OnMeasureItem(HWND hwnd, LPMEASUREITEMSTRUCT lpmis);
static void OnDrawItem(HWND hwnd, LPDRAWITEMSTRUCT lpdis);

typedef struct tagMYITEM
{
    OUString text;
    UINT iconId;
} MYITEM;

static bool isNT()
{
    static bool bInitialized    = false;
    static bool bWnt            = false;

    if( !bInitialized )
    {
        bInitialized = true;

        OSVERSIONINFO   aVerInfo;
        aVerInfo.dwOSVersionInfoSize = sizeof( aVerInfo );
        if ( GetVersionEx( &aVerInfo ) )
        {
            if ( aVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
                bWnt = true;
        }
    }
    return bWnt;
}


static void addMenuItem( HMENU hMenu, UINT id, UINT iconId, OUString& text, int& pos, int bOwnerdraw )
{
    MENUITEMINFOW mi;
    memset( &mi, 0, sizeof( MENUITEMINFOW ) );

    mi.cbSize = sizeof( MENUITEMINFOW );
    if( id == -1 )
    {
        mi.fMask=MIIM_TYPE;
        mi.fType=MFT_SEPARATOR;
    }
    else
    {
        if( bOwnerdraw )
        {
            mi.fMask=MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_DATA;
            mi.fType=MFT_OWNERDRAW;
            mi.fState=MFS_ENABLED;
            mi.wID = id;

            MYITEM *pMyItem = new MYITEM;
            pMyItem->text = text;
            pMyItem->iconId = iconId;
            mi.dwItemData = (DWORD) pMyItem;
        }
        else
        {
            mi.fMask=MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_DATA;
            mi.fType=MFT_STRING;
            mi.fState=MFS_ENABLED;
            mi.wID = id;
            mi.dwTypeData = (LPWSTR) text.getStr();
            mi.cch = text.getLength();
        }
    }

    InsertMenuItemW( hMenu, pos++, TRUE, &mi );
}

static HMENU createSystrayMenu( )
{
    HMENU hMenu = CreatePopupMenu();
    int pos=0;

    ShutdownIcon *pShutdownIcon = ShutdownIcon::getInstance();
    OSL_ENSURE( pShutdownIcon, "ShutdownIcon instance empty!");

    if( !pShutdownIcon )
        return NULL;

    addMenuItem( hMenu, IDM_WRITER, ICON_TEXT_DOCUMENT,
        pShutdownIcon->GetUrlDescription( OUString( RTL_CONSTASCII_USTRINGPARAM ( WRITER_URL ) ) ), pos, true );
    addMenuItem( hMenu, IDM_CALC, ICON_SPREADSHEET_DOCUMENT,
        pShutdownIcon->GetUrlDescription( OUString( RTL_CONSTASCII_USTRINGPARAM ( CALC_URL ) ) ), pos, true );
    addMenuItem( hMenu, IDM_IMPRESS, ICON_PRESENTATION_DOCUMENT,
        pShutdownIcon->GetUrlDescription( OUString( RTL_CONSTASCII_USTRINGPARAM ( IMPRESS_URL ) ) ), pos, true );
    addMenuItem( hMenu, IDM_DRAW, ICON_DRAWING_DOCUMENT,
        pShutdownIcon->GetUrlDescription( OUString( RTL_CONSTASCII_USTRINGPARAM ( DRAW_URL ) ) ), pos, true );
    addMenuItem( hMenu, IDM_TEMPLATE, ICON_TEXT_TEMPLATE,
        pShutdownIcon->GetResString( STR_QUICKSTART_FROMTEMPLATE ), pos, true);
    addMenuItem( hMenu, -1,         0, OUString(), pos, false );
    addMenuItem( hMenu, IDM_OPEN,   0, pShutdownIcon->GetResString( STR_QUICKSTART_FILEOPEN ), pos, false );
    addMenuItem( hMenu, -1,         0, OUString(), pos, false );
    addMenuItem( hMenu, IDM_INSTALL,0, pShutdownIcon->GetResString( STR_QUICKSTART_PRELAUNCH ), pos, false );
    addMenuItem( hMenu, -1,         0, OUString(), pos, false );
    addMenuItem( hMenu, IDM_EXIT,   0, pShutdownIcon->GetResString( STR_QUICKSTART_EXIT ), pos, false );

    // indicate status of autostart folder
    CheckMenuItem( hMenu, IDM_INSTALL, MF_BYCOMMAND | (ShutdownIcon::GetAutostart() ? MF_CHECKED : MF_UNCHECKED) );

    return hMenu;
}

static void deleteSystrayMenu( HMENU hMenu )
{
    if( !hMenu )
        return;

    MENUITEMINFOW mi;
    MYITEM *pMyItem;
    int pos=0;
    memset( &mi, 0, sizeof( mi ) );
    mi.cbSize = sizeof( mi );
    mi.fMask = MIIM_DATA;

    while( GetMenuItemInfoW( hMenu, pos++, true, &mi ) )
    {
        pMyItem = (MYITEM*) mi.dwItemData;
        if( pMyItem )
        {
            pMyItem->text = OUString();
            delete pMyItem;
        }
        mi.fMask = MIIM_DATA;
    }
}

static void addTaskbarIcon( HWND hWnd )
{
    OUString strTip;
    if( ShutdownIcon::getInstance() )
        strTip = ShutdownIcon::getInstance()->GetResString( STR_QUICKSTART_TIP );

    // add taskbar icon
    NOTIFYICONDATAA nid;
    nid.hIcon = (HICON)LoadImageA( GetModuleHandle( NULL ), MAKEINTRESOURCE( ICON_SO_DEFAULT ),
        IMAGE_ICON, GetSystemMetrics( SM_CXSMICON ), GetSystemMetrics( SM_CYSMICON ),
        LR_DEFAULTCOLOR | LR_SHARED );

    // better use unicode wrapper here ?
    strncpy( nid.szTip, ( OUStringToOString(strTip, osl_getThreadTextEncoding()).getStr() ), 64 );

    nid.cbSize              = sizeof(nid);
    nid.hWnd                = hWnd;
    nid.uID                 = ID_QUICKSTART;
    nid.uCallbackMessage    = SFX_TASKBAR_NOTIFICATION;
    nid.uFlags              = NIF_MESSAGE|NIF_TIP|NIF_ICON;

    Shell_NotifyIconA(NIM_ADD, &nid);
}


LRESULT CALLBACK listenerWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HMENU popupMenu = NULL;

    static UINT s_uTaskbarRestart = 0;

    switch (uMsg)
    {
        case WM_NCCREATE:
            return TRUE;
        case WM_CREATE:
            {
                // request notfication when taskbar is recreated
                // we then have to add our icon again
                s_uTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));

                // create the menu
                if( !popupMenu )
                    if( (popupMenu = createSystrayMenu( )) == NULL )
                        return -1;

                // and the icon
                addTaskbarIcon( hWnd );

                // disable shutdown
                ShutdownIcon::getInstance()->SetVeto( true );
            }
            return 0;

        case WM_MEASUREITEM:
            OnMeasureItem(hWnd, (LPMEASUREITEMSTRUCT) lParam);
            return TRUE;

        case WM_DRAWITEM:
            OnDrawItem(hWnd, (LPDRAWITEMSTRUCT) lParam);
            return TRUE;

        case SFX_TASKBAR_NOTIFICATION:
            switch( lParam )
            {
                case WM_LBUTTONDBLCLK:
                    //ShutdownIcon::OpenURL( OUString( RTL_CONSTASCII_USTRINGPARAM( "slot:5500" ) ) );
                    // due to #88164 we cannot open-from-template as default action
                    // so stick with ordinary fileopen
                    ShutdownIcon::FileOpen();
                    break;

                case WM_RBUTTONDOWN:
                {
                    POINT pt;
                    GetCursorPos(&pt);
                    SetForegroundWindow( hWnd );

                    // update status before showing menu, could have been changed from option page
                    CheckMenuItem( popupMenu, IDM_INSTALL, MF_BYCOMMAND| (ShutdownIcon::GetAutostart() ? MF_CHECKED : MF_UNCHECKED) );

                    int m = TrackPopupMenuEx( popupMenu, TPM_RETURNCMD|TPM_LEFTALIGN|TPM_RIGHTBUTTON,
                                              pt.x, pt.y, hWnd, NULL );
                    // BUGFIX: See Q135788 (PRB: Menus for Notification Icons Don't Work Correctly)
                    PostMessage( hWnd, NULL, 0, 0 );
                    switch( m )
                    {
                        case IDM_OPEN:
                            ShutdownIcon::FileOpen();
                        break;
                        case IDM_WRITER:
                            ShutdownIcon::OpenURL( OUString( RTL_CONSTASCII_USTRINGPARAM( WRITER_URL ) ) );
                        break;
                        case IDM_CALC:
                            ShutdownIcon::OpenURL( OUString( RTL_CONSTASCII_USTRINGPARAM( CALC_URL ) ) );
                        break;
                        case IDM_IMPRESS:
                            ShutdownIcon::OpenURL( OUString( RTL_CONSTASCII_USTRINGPARAM( IMPRESS_URL ) ) );
                        break;
                        case IDM_DRAW:
                            ShutdownIcon::OpenURL( OUString( RTL_CONSTASCII_USTRINGPARAM( DRAW_URL ) ) );
                        break;
                        case IDM_TEMPLATE:
                            ShutdownIcon::OpenURL( OUString( RTL_CONSTASCII_USTRINGPARAM( "slot:5500" ) ) );
                        break;
                        case IDM_INSTALL:
                            ShutdownIcon::SetAutostart( !ShutdownIcon::GetAutostart() );
                            CheckMenuItem( popupMenu, IDM_INSTALL, MF_BYCOMMAND| (ShutdownIcon::GetAutostart() ? MF_CHECKED : MF_UNCHECKED) );
                            break;
                        case IDM_EXIT:
                            // delete taskbar icon
                            NOTIFYICONDATAA nid;
                            nid.cbSize=sizeof(NOTIFYICONDATA);
                            nid.hWnd = hWnd;
                            nid.uID = ID_QUICKSTART;
                            Shell_NotifyIconA(NIM_DELETE, &nid);

                            // remove listener and
                            //  terminate office if running in background
                            ShutdownIcon::terminateDesktop();
                            break;
                    }
                }
                break;
            }
            break;
        case WM_DESTROY:
            deleteSystrayMenu( popupMenu );
            return 0;
        default:
            if( uMsg == s_uTaskbarRestart )
            {
                // re-create taskbar icon
                addTaskbarIcon( hWnd );
            }
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

void ShutdownIcon::initSystray()
{

    // initialize unicode wrappers
    User9xInit();

    WNDCLASSEXA listenerClass;
    listenerClass.cbSize        = sizeof(WNDCLASSEX);
    listenerClass.style         = 0;
    listenerClass.lpfnWndProc   = listenerWndProc;
    listenerClass.cbClsExtra    = 0;
    listenerClass.cbWndExtra    = 0;
    listenerClass.hInstance     = (HINSTANCE) GetModuleHandle( NULL );
    listenerClass.hIcon         = NULL;
    listenerClass.hCursor       = NULL;
    listenerClass.hbrBackground = NULL;
    listenerClass.lpszMenuName  = NULL;
    listenerClass.lpszClassName = LISTENER_WINDOWCLASS;
    listenerClass.hIconSm       = NULL;

    RegisterClassExA(&listenerClass);

    aListenerWindow = CreateWindowExA(0,
        LISTENER_WINDOWCLASS,       // registered class name
        LISTENER_WINDOWNAME,        // window name
        0,                          // window style
        CW_USEDEFAULT,              // horizontal position of window
        CW_USEDEFAULT,              // vertical position of window
        CW_USEDEFAULT,              // window width
        CW_USEDEFAULT,              // window height
        (HWND) NULL,                // handle to parent or owner window
        NULL,                       // menu handle or child identifier
        (HINSTANCE) GetModuleHandle( NULL ),    // handle to application instance
        NULL                        // window-creation data
        );
}

void ShutdownIcon::deInitSystray()
{
    if( IsWindow( aListenerWindow ) )
    {
        DestroyWindow( aListenerWindow );
        aListenerWindow = NULL;
    }
    UnregisterClassA( LISTENER_WINDOWCLASS, GetModuleHandle( NULL ) );
}



// -------------------------------

void OnMeasureItem(HWND hwnd, LPMEASUREITEMSTRUCT lpmis)
{
    MYITEM *pMyItem = (MYITEM *) lpmis->itemData;
    HDC hdc = GetDC(hwnd);
    SIZE size;

    NONCLIENTMETRICS ncm;
    memset(&ncm, 0, sizeof(ncm));
    ncm.cbSize = sizeof(ncm);

    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, (PVOID) &ncm, 0);

    HFONT hfntOld = (HFONT) SelectObject(hdc, (HFONT) CreateFontIndirect( &ncm.lfMenuFont ));

    GetTextExtentPoint32W(hdc, pMyItem->text.getStr(),
            pMyItem->text.getLength(), &size);

    lpmis->itemWidth = size.cx + 4 + GetSystemMetrics( SM_CXSMICON );
    lpmis->itemHeight = (size.cy > GetSystemMetrics( SM_CYSMICON )) ? size.cy : GetSystemMetrics( SM_CYSMICON );
    lpmis->itemHeight += 4;

    DeleteObject( SelectObject(hdc, hfntOld) );
    ReleaseDC(hwnd, hdc);
}

void OnDrawItem(HWND hwnd, LPDRAWITEMSTRUCT lpdis)
{
    MYITEM *pMyItem = (MYITEM *) lpdis->itemData;
    COLORREF clrPrevText, clrPrevBkgnd;
    HFONT hfntOld;
    HBRUSH hbrOld;
    int x, y;

    // Set the appropriate foreground and background colors.

    RECT aRect = lpdis->rcItem;

    if (lpdis->itemState & ODS_SELECTED)
    {
        clrPrevText = SetTextColor(lpdis->hDC,
                GetSysColor(COLOR_HIGHLIGHTTEXT));
        clrPrevBkgnd = SetBkColor(lpdis->hDC,
                GetSysColor(COLOR_HIGHLIGHT));
        hbrOld = (HBRUSH) SelectObject(lpdis->hDC,
            CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT)));
    }
    else
    {
        clrPrevText = SetTextColor(lpdis->hDC,
                GetSysColor(COLOR_MENUTEXT));
        clrPrevBkgnd = SetBkColor(lpdis->hDC,
                GetSysColor(COLOR_MENU));
        hbrOld = (HBRUSH) SelectObject(lpdis->hDC,
            CreateSolidBrush(GetSysColor(COLOR_MENU)));
    }

    // Fill background
    PatBlt(lpdis->hDC, aRect.left, aRect.top, aRect.right-aRect.left, aRect.bottom-aRect.top, PATCOPY);

    int height = aRect.bottom-aRect.top;

    x = aRect.left;
    y = aRect.top;

    int cx = GetSystemMetrics( SM_CXSMICON );
    int cy = GetSystemMetrics( SM_CYSMICON );
    HICON hIcon = (HICON) LoadImageA( GetModuleHandle( NULL ), MAKEINTRESOURCE( pMyItem->iconId ),
                                      IMAGE_ICON, cx, cy,
                                      LR_DEFAULTCOLOR | LR_SHARED );

    DrawIconEx( lpdis->hDC, x, y+(height-cy)/2, hIcon, cx, cy, 0, NULL, DI_NORMAL );

    x += cx + 4;    // space for icon
    aRect.left = x;

    NONCLIENTMETRICS ncm;
    memset(&ncm, 0, sizeof(ncm));
    ncm.cbSize = sizeof(ncm);

    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, (PVOID) &ncm, 0);
    hfntOld = (HFONT) SelectObject(lpdis->hDC, (HFONT) CreateFontIndirect( &ncm.lfMenuFont ));


    SIZE size;
    GetTextExtentPoint32W(lpdis->hDC, pMyItem->text.getStr(),
            pMyItem->text.getLength(), &size);

    ExtTextOutW(lpdis->hDC, aRect.left, aRect.top + (height - size.cy)/2, ETO_OPAQUE,
            &aRect, pMyItem->text.getStr(),
            pMyItem->text.getLength(), NULL);


    // Restore the original font and colors.
    DeleteObject( SelectObject( lpdis->hDC, hbrOld ) );
    DeleteObject( SelectObject( lpdis->hDC, hfntOld) );
    SetTextColor(lpdis->hDC, clrPrevText);
    SetBkColor(lpdis->hDC, clrPrevBkgnd);
}

// -------------------------------
// code from setup2 project
// -------------------------------

void _SHFree( void *pv )
{
    IMalloc *pMalloc;
    if( NOERROR == SHGetMalloc(&pMalloc) )
    {
        pMalloc->Free( pv );
        pMalloc->Release();
    }
}

#define ALLOC(type, n) ((type *) HeapAlloc(GetProcessHeap(), 0, sizeof(type) * n ))
#define FREE(p) HeapFree(GetProcessHeap(), 0, p)

static OUString _SHGetSpecialFolder( int nFolderID )
{

    LPITEMIDLIST    pidl;
    HRESULT         hHdl = SHGetSpecialFolderLocation( NULL, nFolderID, &pidl );
    OUString        aFolder;

    if( hHdl == NOERROR )
    {
        WCHAR *lpFolderA;
        lpFolderA = ALLOC( WCHAR, 16000 );

        SHGetPathFromIDListW( pidl, lpFolderA );
        aFolder = OUString( lpFolderA );

        FREE( lpFolderA );
        _SHFree( pidl );
    }
    return aFolder;
}

static OUString SHGetAutostartFolderName()
{
    return _SHGetSpecialFolder(CSIDL_STARTUP);
}

static HRESULT WINAPI SHCoCreateInstance( LPVOID lpszReserved, REFCLSID clsid, LPUNKNOWN pUnkUnknown, REFIID iid, LPVOID *ppv )
{
    HRESULT hResult = E_NOTIMPL;
    HMODULE hModShell = GetModuleHandle( "SHELL32" );

    if ( hModShell != NULL )
    {
        typedef HRESULT (WINAPI *SHCoCreateInstance_PROC)( LPVOID lpszReserved, REFCLSID clsid, LPUNKNOWN pUnkUnknwon, REFIID iid, LPVOID *ppv );

        SHCoCreateInstance_PROC lpfnSHCoCreateInstance = (SHCoCreateInstance_PROC)GetProcAddress( hModShell, MAKEINTRESOURCE(102) );

        if ( lpfnSHCoCreateInstance )
            hResult = lpfnSHCoCreateInstance( lpszReserved, clsid, pUnkUnknown, iid, ppv );
    }
    return hResult;
}

BOOL CreateShortcut( const OUString& rAbsObject, const OUString& rAbsObjectPath,
    const OUString& rAbsShortcut, const OUString& rDescription, const OUString& rParameter )
{
    HRESULT hres;
    IShellLink* psl;
    CLSID clsid_ShellLink = CLSID_ShellLink;
    CLSID clsid_IShellLink = IID_IShellLink;

    hres = CoCreateInstance( clsid_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                             clsid_IShellLink, (void**)&psl );
    if( FAILED(hres) )
        hres = SHCoCreateInstance( NULL, clsid_ShellLink, NULL, clsid_IShellLink, (void**)&psl );

    if( SUCCEEDED(hres) )
    {
        IPersistFile* ppf;
        psl->SetPath( OUStringToOString(rAbsObject, osl_getThreadTextEncoding()).getStr() );
        psl->SetWorkingDirectory( OUStringToOString(rAbsObjectPath, osl_getThreadTextEncoding()).getStr() );
        psl->SetDescription( OUStringToOString(rDescription, osl_getThreadTextEncoding()).getStr() );
        if( rParameter.getLength() )
            psl->SetArguments( OUStringToOString(rParameter, osl_getThreadTextEncoding()).getStr() );

        CLSID clsid_IPersistFile = IID_IPersistFile;
        hres = psl->QueryInterface( clsid_IPersistFile, (void**)&ppf );

        if( SUCCEEDED(hres) )
        {
            hres = ppf->Save( rAbsShortcut.getStr(), TRUE );
            ppf->Release();
        } else return FALSE;
        psl->Release();
    } else return FALSE;
    return TRUE;
}

// ------------------
// install/uninstall

void ShutdownIcon::SetAutostartW32( const OUString& aShortcutName, bool bActivate )
{
    // init wrapper if necessary
    User9xInit();

    OUString aShortcut(SHGetAutostartFolderName());
    aShortcut += OUString( RTL_CONSTASCII_USTRINGPARAM( "\\" ) );
    aShortcut += aShortcutName;

    if( bActivate )
    {
        wchar_t aPath[_MAX_PATH];
        if( isNT() )
        {
            GetModuleFileNameW( NULL, aPath, _MAX_PATH-1);
        }
        else
        {
            char szPathA[_MAX_PATH];
            int len = GetModuleFileNameA( NULL, szPathA, _MAX_PATH-1);

            // calc the string wcstr len
            int nNeededWStrBuffSize = MultiByteToWideChar( CP_ACP, 0, szPathA, -1, NULL, 0 );

            // copy the string if necessary
            if ( nNeededWStrBuffSize > 0 )
                MultiByteToWideChar( CP_ACP, 0, szPathA, -1, aPath, nNeededWStrBuffSize );
        }

        OUString aOfficepath( aPath );
        int i = aOfficepath.lastIndexOf((sal_Char) '\\');
        if( i != -1 )
            aOfficepath = aOfficepath.copy(0, i);

        OUString quickstartExe(aOfficepath);
        quickstartExe += OUString( RTL_CONSTASCII_USTRINGPARAM( "\\quickstart.exe" ) );

        CreateShortcut( quickstartExe, aOfficepath, aShortcut, OUString(), OUString() );
    }
    else
    {
        OUString aShortcutUrl;
        ::osl::File::getFileURLFromSystemPath( aShortcut, aShortcutUrl );
        ::osl::File::remove( aShortcutUrl );
    }
}

bool ShutdownIcon::GetAutostartW32( const OUString& aShortcutName )
{
    // init wrapper if necessary
    User9xInit();

    OUString aShortcut(SHGetAutostartFolderName());
    aShortcut += OUString( RTL_CONSTASCII_USTRINGPARAM( "\\" ) );
    aShortcut += aShortcutName;

    OUString aShortcutUrl;
    File::getFileURLFromSystemPath( aShortcut, aShortcutUrl );
    File f( aShortcutUrl );
    File::RC error = f.open( OpenFlag_Read );
    if( error == File::E_None )
    {
        f.close();
        return true;
    }
    else
        return false;

}
#endif // WNT


