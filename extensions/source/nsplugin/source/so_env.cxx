/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: so_env.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: ihi $ $Date: 2008-01-14 14:45:13 $
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
#include "precompiled_extensions.hxx"

#ifdef UNIX
#include <sys/types.h>
#include <strings.h>
#if defined(SOLARIS) && !defined(__GNUC__)
#include <varargs.h>
#else
#include <stdarg.h>
#endif
// For vsnprintf()
#define NSP_vsnprintf vsnprintf
#include "nsp_func.hxx"
#endif // End UNIX

#ifdef WNT
#define _WINDOWS

#pragma warning (push,1)
#pragma warning (disable:4668)
#pragma warning (disable:4917)

#include <windows.h>
#include <direct.h>
#include <stdlib.h>
#include <shlobj.h>
#include <objidl.h>
// For vsnprintf()
#define NSP_vsnprintf _vsnprintf

#pragma warning (pop)
#endif // End WNT

#include <sys/stat.h>
#include <errno.h>
#include "so_env.hxx"
#include "ns_debug.hxx"
#include <sal/config.h>

#define PLUGIN_NAME         "OpenOffice.org"

// Tranform all strings like %20 in pPath to one char like space
/*int retoreUTF8(char* pPath)
{
    // Prepare buf
    int len = strlen(pPath) + 1;
    char* pBuf = (char*)malloc(len);
    memset(pBuf, 0, len);

    // Store the original pBuf and pPath
    char* pBufCur = pBuf;
    char* pPathCur = pPath;
    // ie, for %20, UTF8Numbers[0][0] = 2, UTF8Numbers[1][0] = 0
    char UTF8Numbers[2][2] = {{0, 0}, {0,0}};
    int temp;

    while (*pPathCur) {
        if (('%' == *pPathCur) && (0 != *(pPathCur + 1))
            && (0 != *(pPathCur + 2)))
        {
            UTF8Numbers[0][0] = *(pPathCur + 1);
            UTF8Numbers[1][0] = *(pPathCur + 2);
            temp = 0;
            temp = atoi(UTF8Numbers[0])*16 + atoi(UTF8Numbers[1]);
            *pBufCur = (char)temp;
            pBufCur++;
            pPathCur += 3;
        } else {
            *pBufCur++ = *pPathCur++;
        }
    }

    *pBufCur = 0;
    strcpy(pPath, pBuf);
    free(pBuf);
    return 0;
}*/

int
restoreUTF8(char *pPath)
{
    unsigned char *s, *d;

#define XDIGIT(c) ((c) <= '9' ? (c) - '0' : ((c) & 0x4F) - 'A' + 10)

    s = d = (unsigned char *)pPath;
    do {
        if (*s == '%' && s[1] && s[2]) {
            *d++ = (XDIGIT (s[1]) << 4) + XDIGIT (s[2]);
            s += 2;
        } else
            *d++ = *s;
    } while (*s++);
    debug_fprintf(NSP_LOG_APPEND, "after restoreUTF8, pPath is %s\n", pPath);
    return 0;
}

// *aResult points the static string holding "/opt/staroffice8"
int findReadSversion(void** aResult, int /*bWnt*/, const char* /*tag*/, const char* /*entry*/)
{
#ifdef UNIX
    // The real space to hold "/opt/staroffice8"
    static char realFileName[NPP_PATH_MAX] = {0};
    memset(realFileName, 0, NPP_PATH_MAX);
    *aResult = realFileName;

    // Filename of lnk file, eg. "soffice"
    char lnkFileName[NPP_PATH_MAX] = {0};
    char* pTempZero = NULL;
    sprintf(lnkFileName, "%s/.mozilla/plugins/libnpsoplugin%s", getenv("HOME"), SAL_DLLEXTENSION);
    if ((0 > readlink(lnkFileName, realFileName, NPP_PATH_MAX)) ||
       (NULL == (pTempZero = strstr(realFileName, "/program/libnpsoplugin" SAL_DLLEXTENSION))))
    {
        *realFileName = 0;
        return -1;
    }
    *pTempZero = 0;
    return 0;
#endif
#ifdef WNT
    static char realFileName[NPP_PATH_MAX] = {0};
    *aResult = realFileName;
    HKEY hKey;
    DWORD dwBufLen = NPP_PATH_MAX;
    LONG lRet;

    debug_fprintf(NSP_LOG_APPEND, "1 before before strstr realFileName is %s\n", realFileName);
    lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
       "SOFTWARE\\MozillaPlugins\\@sun.com/npsopluginmi;version=1.0",
       0, KEY_QUERY_VALUE, &hKey );
    debug_fprintf(NSP_LOG_APPEND, "2 before before strstr realFileName is %s\n", realFileName);
    if( lRet != ERROR_SUCCESS )
       return FALSE;

    lRet = RegQueryValueEx( hKey, "Path", NULL, NULL,
       (LPBYTE) realFileName, &dwBufLen);
    debug_fprintf(NSP_LOG_APPEND, "3 before before strstr realFileName is %s\n", realFileName);
    if( (lRet != ERROR_SUCCESS) || (dwBufLen > NPP_PATH_MAX) )
       return FALSE;

    RegCloseKey( hKey );
    char* pTempZero = NULL;
    debug_fprintf(NSP_LOG_APPEND, "before strstr realFileName is %s\n", realFileName);
    if (NULL == (pTempZero = strstr(realFileName, "\\program")))
    {
        *realFileName = 0;
        return -1;
    }
    *pTempZero = 0;
    debug_fprintf(NSP_LOG_APPEND, "realFileName is %s\n", realFileName);
    return 0;
#endif
}

// Return the install dir path of staroffice, return value like "/home/build/staroffice"
const char* findInstallDir()
{
    static char* pInstall = NULL;
    debug_fprintf(NSP_LOG_APPEND, "start of findInstallDir()\n");
    if (!pInstall)
    {
        findReadSversion((void**)&pInstall, 0, "["SECTION_NAME"]", SOFFICE_VERSION"=");
        if (!pInstall)
            pInstall = const_cast< char* >( "" );
    }
    return pInstall;
}

// Return the program dir path of staroffice, return value like "/home/build/staroffice/program"
const char* findProgramDir()
{
    static char sProgram[NPP_BUFFER_SIZE] = {0};
    if (!sProgram[0])
    {
        sprintf(sProgram, "%s/program", findInstallDir());
#ifdef WNT
        UnixToDosPath(sProgram);
#endif
    }
    return sProgram;
}

// Return the nsplugin executable file path, return value like "/home/build/staroffice/program/nsplugin"
const char* findNsExeFile()
{
    static char sNsExe[NPP_BUFFER_SIZE] = {0};

    if (!sNsExe[0])
    {
        sprintf(sNsExe, "%s/%s", findProgramDir(), PLUGIN_EXE_FILE_NAME);
#ifdef WNT
        UnixToDosPath(sNsExe);
#endif
     }
    return sNsExe;
}

// Return: "/home/build/staroffice/program" + original system library path
const char* getNewLibraryPath()
{
    static char pLPATH[NPP_PATH_MAX*4] = {0};

    if (!pLPATH[0])
    {
        const char* pProgram = findProgramDir();
        strcpy(pLPATH, "LD_LIBRARY_PATH=");
        strcat(pLPATH, pProgram);

        char* pLD = getenv("LD_LIBRARY_PATH");
        if (pLD)
        {
            strcat(pLPATH, ":");
            strcat(pLPATH, pLD);
        }
    }
    return pLPATH;
}


// Return SO executable absolute path, like "/home/build/staroffice/program/soffice"
const char* findSofficeExecutable()
{
    static char pSofficeExeccutable[NPP_PATH_MAX] = {0};
    if (!pSofficeExeccutable[0])
    {
        sprintf(pSofficeExeccutable, "%s/%s", findProgramDir(), STAROFFICE_EXE_FILE_NAME);
#ifdef WNT
        UnixToDosPath(pSofficeExeccutable);
#endif
    }

    return pSofficeExeccutable;
}

// Change Dos path such as c:\program\soffice to c:/program/soffice
int DosToUnixPath(char* sPath)
{
    if (!sPath)
        return -1;
    char* p = sPath;
    while (*p)
    {
        if(*p == '\\')
            *p = '/';
        p++;
    }
    return 0;

}

// Change Unix path such as program/soffice to program\soffice
int UnixToDosPath(char* sPath)
{
    if (!sPath)
        return -1;
    char* p = sPath;
    while (*p)
    {
        if(*p == '/')
            *p = '\\';
        p++;
    }
    return 0;

}

#ifdef UNIX
char productName[128] = {0};
char* NSP_getProductName()
{
    if(productName[0])
        return productName;
    char fullBootstrapIniPath[1024] = {0};
    const char* pFullFilePath = findProgramDir();
    if(0 == *pFullFilePath)
    {
        strcpy(productName, PLUGIN_NAME);
        return productName;
    }
    sprintf(fullBootstrapIniPath, "%s/%s", pFullFilePath,
        "bootstraprc");

    FILE* fp = fopen(fullBootstrapIniPath, "r");

    if (NULL == fp)
    {
        strcpy(productName, PLUGIN_NAME);
        return productName;
    }
    char line[4096] = {0};
    char *pStart = 0;
    char *pEnd = 0;
    while(!feof(fp))
    {
        fgets( line, sizeof(line), fp );
        if (NULL == (pStart = strstr( line, "ProductKey=" )))
            continue;
        pStart += strlen("ProductKey=");
        if (NULL == (pEnd = strchr( pStart, ' ' )) &&
           (NULL == (pEnd = strchr( pStart, '\r' ))))
            continue;
        *pEnd = 0;
        strcpy(productName, pStart);
    }
    fclose(fp);
    if ((*productName == 0) ||
        (0 != STRNICMP(productName, "StarOffice", sizeof("StarOffice"))))
    {
        strcpy(productName, PLUGIN_NAME);
        return productName;
    }
    memset(productName, 0, sizeof(productName));
    strcat(productName, "StarOffice/StarSuite");
    return productName;
}

char PluginName[1024] = {0};
char* NSP_getPluginName()
{
    if(*PluginName)
        return PluginName;
    sprintf(PluginName, "%s Plug-in", NSP_getProductName());
    return PluginName;
}

char PluginDesc[1024] = {0};
char* NSP_getPluginDesc()
{
    if(*PluginDesc)
        return PluginDesc;

    sprintf(PluginDesc, "%s Plug-in handles all its documents",
        productName);
    return PluginDesc;
}
#endif //end of UNIX

void NSP_WriteLog(int level,  const char* pFormat, ...)
{
#ifndef DEBUG
    (void)level;
    (void)pFormat;
#else
    va_list      ap;
    char         msgBuf[NPP_BUFFER_SIZE];
    static char  logName[NPP_PATH_MAX] = {0};
    FILE *       fp = NULL;
    int          dwPos,dwWriteBytes;

    va_start(ap,pFormat);
    NSP_vsnprintf(msgBuf, NPP_BUFFER_SIZE, pFormat, ap);
    va_end(ap);

    if (!logName[0])
    {
#ifdef UNIX
        const char* homeDir = getenv("HOME");
        sprintf(logName,"%s/%s",homeDir,"nsplugin.log");
#endif // End UNIX
#ifdef WNT
        char szPath[MAX_PATH];
        if (!SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, 0))
        {
             return;
        }
        char* homeDir = szPath;
        sprintf(logName,"%s\\%s", szPath, "nsplugin.log");
#endif // End WNT
    }
    else
        fp = fopen(logName, "a+");

    if (!fp)
        return;
    fputs(msgBuf, fp);
    fclose(fp);
#endif
}

