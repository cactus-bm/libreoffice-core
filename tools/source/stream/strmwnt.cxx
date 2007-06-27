/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: strmwnt.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 22:18:59 $
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

// no include "precompiled_tools.hxx" because this file is included in strmsys.cxx

/*
    Todo: StreamMode <-> AllocateMemory
*/

#include <string.h>
#include <limits.h>

#include <tools/svwin.h>

#include <tools/debug.hxx>
#include <tools/fsys.hxx>
#include <tools/stream.hxx>

// class FileBase
#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif
using namespace osl;

// -----------------------------------------------------------------------

// --------------
// - StreamData -
// --------------

class StreamData
{
public:
    HANDLE      hFile;

                StreamData()
                {
                    hFile = 0;
                }
};

// -----------------------------------------------------------------------

static ULONG GetSvError( DWORD nWntError )
{
    static struct { DWORD wnt; ULONG sv; } errArr[] =
    {
        { ERROR_SUCCESS,                SVSTREAM_OK },
        { ERROR_ACCESS_DENIED,          SVSTREAM_ACCESS_DENIED },
        { ERROR_ACCOUNT_DISABLED,       SVSTREAM_ACCESS_DENIED },
        { ERROR_ACCOUNT_EXPIRED,        SVSTREAM_ACCESS_DENIED },
        { ERROR_ACCOUNT_RESTRICTION,    SVSTREAM_ACCESS_DENIED },
        { ERROR_ATOMIC_LOCKS_NOT_SUPPORTED, SVSTREAM_INVALID_PARAMETER },
        { ERROR_BAD_PATHNAME,           SVSTREAM_PATH_NOT_FOUND },
        // Filename too long
        { ERROR_BUFFER_OVERFLOW,        SVSTREAM_INVALID_PARAMETER },
        { ERROR_DIRECTORY,              SVSTREAM_INVALID_PARAMETER },
        { ERROR_DRIVE_LOCKED,           SVSTREAM_LOCKING_VIOLATION },
        { ERROR_FILE_NOT_FOUND,         SVSTREAM_FILE_NOT_FOUND },
        { ERROR_FILENAME_EXCED_RANGE,   SVSTREAM_INVALID_PARAMETER },
        { ERROR_INVALID_ACCESS,         SVSTREAM_INVALID_ACCESS },
        { ERROR_INVALID_DRIVE,          SVSTREAM_PATH_NOT_FOUND },
        { ERROR_INVALID_HANDLE,         SVSTREAM_INVALID_HANDLE },
        { ERROR_INVALID_NAME,           SVSTREAM_PATH_NOT_FOUND },
        { ERROR_INVALID_PARAMETER,      SVSTREAM_INVALID_PARAMETER },
        { ERROR_IS_SUBST_PATH,          SVSTREAM_INVALID_PARAMETER },
        { ERROR_IS_SUBST_TARGET,        SVSTREAM_INVALID_PARAMETER },
        { ERROR_LOCK_FAILED,            SVSTREAM_LOCKING_VIOLATION },
        { ERROR_LOCK_VIOLATION,         SVSTREAM_LOCKING_VIOLATION },
        { ERROR_NEGATIVE_SEEK,          SVSTREAM_SEEK_ERROR },
        { ERROR_PATH_NOT_FOUND,         SVSTREAM_PATH_NOT_FOUND },
        { ERROR_READ_FAULT,             SVSTREAM_READ_ERROR },
        { ERROR_SEEK,                   SVSTREAM_SEEK_ERROR },
        { ERROR_SEEK_ON_DEVICE,         SVSTREAM_SEEK_ERROR },
        { ERROR_SHARING_BUFFER_EXCEEDED,SVSTREAM_SHARE_BUFF_EXCEEDED },
        { ERROR_SHARING_PAUSED,         SVSTREAM_SHARING_VIOLATION },
        { ERROR_SHARING_VIOLATION,      SVSTREAM_SHARING_VIOLATION },
        { ERROR_TOO_MANY_OPEN_FILES,    SVSTREAM_TOO_MANY_OPEN_FILES },
        { ERROR_WRITE_FAULT,            SVSTREAM_WRITE_ERROR },
        { ERROR_WRITE_PROTECT,          SVSTREAM_ACCESS_DENIED },
        { ERROR_DISK_FULL,              SVSTREAM_DISK_FULL },

        { (DWORD)0xFFFFFFFF, SVSTREAM_GENERALERROR }
    };

    ULONG nRetVal = SVSTREAM_GENERALERROR;    // Standardfehler
    int i=0;
    do
    {
        if( errArr[i].wnt == nWntError )
        {
            nRetVal = errArr[i].sv;
            break;
        }
        i++;
    } while( errArr[i].wnt != (DWORD)0xFFFFFFFF );
    return nRetVal;
}

/*************************************************************************
|*
|*    SvFileStream::SvFileStream()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 17.06.94
|*    Letzte Aenderung  TPF 15.07.98
|*
*************************************************************************/

SvFileStream::SvFileStream( const String& rFileName, StreamMode nMode )
{
    bIsOpen             = FALSE;
    nLockCounter        = 0;
    bIsWritable         = FALSE;
    pInstanceData       = new StreamData;

    SetBufferSize( 8192 );
    // convert URL to SystemPath, if necessary
    ::rtl::OUString aFileName, aNormPath;

    if ( FileBase::getSystemPathFromFileURL( rFileName, aFileName ) != FileBase::E_None )
        aFileName = rFileName;
    Open( aFileName, nMode );
}

/*************************************************************************
|*
|*    SvFileStream::SvFileStream()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 22.11.94
|*    Letzte Aenderung  TPF 15.07.98
|*
*************************************************************************/

SvFileStream::SvFileStream()
{
    bIsOpen             = FALSE;
    nLockCounter        = 0;
    bIsWritable         = FALSE;
    pInstanceData       = new StreamData;

    SetBufferSize( 8192 );
}

/*************************************************************************
|*
|*    SvFileStream::~SvFileStream()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 14.06.94
|*    Letzte Aenderung  OV 14.06.94
|*
*************************************************************************/

SvFileStream::~SvFileStream()
{
    Close();
    if (pInstanceData)
        delete pInstanceData;
}

/*************************************************************************
|*
|*    SvFileStream::GetFileHandle()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 14.06.94
|*    Letzte Aenderung  OV 14.06.94
|*
*************************************************************************/

ULONG SvFileStream::GetFileHandle() const
{
    return (ULONG)pInstanceData->hFile;
}

/*************************************************************************
|*
|*    SvFileStream::IsA()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 14.06.94
|*    Letzte Aenderung  OV 14.06.94
|*
*************************************************************************/

USHORT SvFileStream::IsA() const
{
    return ID_FILESTREAM;
}

/*************************************************************************
|*
|*    SvFileStream::GetData()
|*
|*    Beschreibung      STREAM.SDW, Prueft nicht Eof; IsEof danach rufbar
|*    Ersterstellung    OV 15.06.94
|*    Letzte Aenderung  TPF 15.07.98
|*
*************************************************************************/

ULONG SvFileStream::GetData( void* pData, ULONG nSize )
{
    DWORD nCount = 0;
    if( IsOpen() )
    {
        bool bResult = ReadFile(pInstanceData->hFile,(LPVOID)pData,nSize,&nCount,NULL);
        if( !bResult )
        {
            ULONG nTestError = GetLastError();
            SetError(::GetSvError( nTestError ) );
        }
    }
    return (DWORD)nCount;
}

/*************************************************************************
|*
|*    SvFileStream::PutData()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 15.06.94
|*    Letzte Aenderung  TPF 15.07.98
|*
*************************************************************************/

ULONG SvFileStream::PutData( const void* pData, ULONG nSize )
{
    DWORD nCount = 0;
    if( IsOpen() )
    {
        if(!WriteFile(pInstanceData->hFile,(LPVOID)pData,nSize,&nCount,NULL))
            SetError(::GetSvError( GetLastError() ) );
    }
    return nCount;
}

/*************************************************************************
|*
|*    SvFileStream::SeekPos()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 15.06.94
|*    Letzte Aenderung  TPF 15.07.98
|*
*************************************************************************/

ULONG SvFileStream::SeekPos( ULONG nPos )
{
    DWORD nNewPos = 0;
    if( IsOpen() )
    {
        if( nPos != STREAM_SEEK_TO_END )
            // 64-Bit files werden nicht unterstuetzt
            nNewPos=SetFilePointer(pInstanceData->hFile,nPos,NULL,FILE_BEGIN);
        else
            nNewPos=SetFilePointer(pInstanceData->hFile,0L,NULL,FILE_END);

        if( nNewPos == 0xFFFFFFFF )
        {
            SetError(::GetSvError( GetLastError() ) );
            nNewPos = 0L;
        }
    }
    else
        SetError( SVSTREAM_GENERALERROR );
    return (ULONG)nNewPos;
}

/*************************************************************************
|*
|*    SvFileStream::Tell()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 15.06.94
|*    Letzte Aenderung  OV 15.06.94
|*
*************************************************************************/
/*
ULONG SvFileStream::Tell()
{
    ULONG nPos = 0L;

    if( IsOpen() )
    {
        DWORD nPos;
        nPos = SetFilePointer(pInstanceData->hFile,0L,NULL,FILE_CURRENT);
        if( nPos = 0xFFFFFFFF )
        {
            SetError( ::GetSvError( GetLastError() ) );
            nPos = 0L;
        }
    }
    return nPos;
}
*/

/*************************************************************************
|*
|*    SvFileStream::FlushData()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 15.06.94
|*    Letzte Aenderung  TPF 15.07.98
|*
*************************************************************************/

void SvFileStream::FlushData()
{
    if( IsOpen() )
    {
        if( !FlushFileBuffers(pInstanceData->hFile) )
            SetError(::GetSvError(GetLastError()));
    }
}

/*************************************************************************
|*
|*    SvFileStream::LockRange()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 15.06.94
|*    Letzte Aenderung  TPF 15.07.98
|*
*************************************************************************/

BOOL SvFileStream::LockRange( ULONG nByteOffset, ULONG nBytes )
{
    bool bRetVal = false;
    if( IsOpen() )
    {
        bRetVal = ::LockFile(pInstanceData->hFile,nByteOffset,0L,nBytes,0L );
        if( !bRetVal )
            SetError(::GetSvError(GetLastError()));
    }
    return bRetVal;
}

/*************************************************************************
|*
|*    SvFileStream::UnlockRange()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 15.06.94
|*    Letzte Aenderung  TPF 15.07.98
|*
*************************************************************************/

BOOL SvFileStream::UnlockRange( ULONG nByteOffset, ULONG nBytes )
{
    bool bRetVal = false;
    if( IsOpen() )
    {
        bRetVal = ::UnlockFile(pInstanceData->hFile,nByteOffset,0L,nBytes,0L );
        if( !bRetVal )
            SetError(::GetSvError(GetLastError()));
    }
    return bRetVal;
}

/*************************************************************************
|*
|*    SvFileStream::LockFile()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 15.06.94
|*    Letzte Aenderung  OV 15.06.94
|*
*************************************************************************/

BOOL SvFileStream::LockFile()
{
    BOOL bRetVal = FALSE;
    if( !nLockCounter )
    {
        if( LockRange( 0L, LONG_MAX ) )
        {
            nLockCounter = 1;
            bRetVal = TRUE;
        }
    }
    else
    {
        nLockCounter++;
        bRetVal = TRUE;
    }
    return bRetVal;
}

/*************************************************************************
|*
|*    SvFileStream::UnlockFile()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 15.06.94
|*    Letzte Aenderung  OV 15.06.94
|*
*************************************************************************/

BOOL SvFileStream::UnlockFile()
{
    BOOL bRetVal = FALSE;
    if( nLockCounter > 0)
    {
        if( nLockCounter == 1)
        {
            if( UnlockRange( 0L, LONG_MAX ) )
            {
                nLockCounter = 0;
                bRetVal = TRUE;
            }
        }
        else
        {
            nLockCounter--;
            bRetVal = TRUE;
        }
    }
    return bRetVal;
}


/*************************************************************************
|*
|*    SvFileStream::Open()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 15.06.94
|*    Letzte Aenderung  TPF 15.07.98
|*
*************************************************************************/
/*
      NOCREATE       TRUNC   NT-Action
      ----------------------------------------------
         0 (Create)    0     OPEN_ALWAYS
         0 (Create)    1     CREATE_ALWAYS
         1             0     OPEN_EXISTING
         1             1     TRUNCATE_EXISTING
*/

void SvFileStream::Open( const String& rFilename, StreamMode nMode )
{
    String aParsedFilename(rFilename);

    SetLastError( ERROR_SUCCESS );
    Close();
    SvStream::ClearBuffer();

    eStreamMode = nMode;
    eStreamMode &= ~STREAM_TRUNC; // beim ReOpen nicht cutten

    //    !!! NoOp: Ansonsten ToAbs() verwendern
    //    !!! DirEntry aDirEntry( rFilename );
    //    !!! aFilename = aDirEntry.GetFull();
    aFilename = aParsedFilename;
#ifdef BOOTSTRAP
    ByteString aFileNameA( aFilename, gsl_getSystemTextEncoding());
#else
    ByteString aFileNameA( aFilename, osl_getThreadTextEncoding());
    FSysRedirector::DoRedirect( aFilename );
#endif
    SetLastError( ERROR_SUCCESS );  // ggf. durch Redirector geaendert!

    /*
    #ifdef DBG_UTIL
    String aTraceStr( "SvFileStream::Open(): " );
    aTraceStr += aFilename;
    DBG_TRACE( aTraceStr );
    #endif
    */

    DWORD   nOpenAction;
    DWORD   nShareMode      = FILE_SHARE_READ | FILE_SHARE_WRITE;
    DWORD   nAccessMode     = 0L;
    UINT    nOldErrorMode = SetErrorMode( SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX );

    if( nMode & STREAM_SHARE_DENYREAD)
        nShareMode &= ~FILE_SHARE_READ;

    if( nMode & STREAM_SHARE_DENYWRITE)
        nShareMode &= ~FILE_SHARE_WRITE;

    if( nMode & STREAM_SHARE_DENYALL)
        nShareMode = 0;

    if( (nMode & STREAM_READ) )
        nAccessMode |= GENERIC_READ;
    if( (nMode & STREAM_WRITE) )
        nAccessMode |= GENERIC_WRITE;

    if( nAccessMode == GENERIC_READ )       // ReadOnly ?
        nMode |= STREAM_NOCREATE;   // wenn ja, nicht erzeugen

    // Zuordnung siehe obige Wahrheitstafel
    if( !(nMode & STREAM_NOCREATE) )
    {
        if( nMode & STREAM_TRUNC )
            nOpenAction = CREATE_ALWAYS;
        else
            nOpenAction = OPEN_ALWAYS;
    }
    else
    {
        if( nMode & STREAM_TRUNC )
            nOpenAction = TRUNCATE_EXISTING;
        else
            nOpenAction = OPEN_EXISTING;
    }

    pInstanceData->hFile = CreateFile(
        aFileNameA.GetBuffer(),
        nAccessMode,
        nShareMode,
        (LPSECURITY_ATTRIBUTES)NULL,
        nOpenAction,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
        (HANDLE) NULL
    );

    if(  pInstanceData->hFile!=INVALID_HANDLE_VALUE && (
        // Hat Create Always eine existierende Datei ueberschrieben ?
        GetLastError() == ERROR_ALREADY_EXISTS ||
        // Hat Open Always eine neue Datei angelegt ?
        GetLastError() == ERROR_FILE_NOT_FOUND  ))
    {
        // wenn ja, dann alles OK
        if( nOpenAction == OPEN_ALWAYS || nOpenAction == CREATE_ALWAYS )
            SetLastError( ERROR_SUCCESS );
    }

    // Bei Fehler pruefen, ob wir lesen duerfen
    if( (pInstanceData->hFile==INVALID_HANDLE_VALUE) &&
         (nAccessMode & GENERIC_WRITE))
    {
        ULONG nErr = ::GetSvError( GetLastError() );
        if(nErr==SVSTREAM_ACCESS_DENIED || nErr==SVSTREAM_SHARING_VIOLATION)
        {
            nMode &= (~STREAM_WRITE);
            nAccessMode = GENERIC_READ;
            // OV, 28.1.97: Win32 setzt die Datei auf 0-Laenge, wenn
            // die Openaction CREATE_ALWAYS ist!!!!
            nOpenAction = OPEN_EXISTING;
            SetLastError( ERROR_SUCCESS );
            pInstanceData->hFile = CreateFile(
                aFileNameA.GetBuffer(),
                GENERIC_READ,
                nShareMode,
                (LPSECURITY_ATTRIBUTES)NULL,
                nOpenAction,
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
                (HANDLE) NULL
            );
            if( GetLastError() == ERROR_ALREADY_EXISTS )
                SetLastError( ERROR_SUCCESS );
        }
    }

    if( GetLastError() != ERROR_SUCCESS )
    {
        bIsOpen = FALSE;
        SetError(::GetSvError( GetLastError() ) );
    }
    else
    {
        bIsOpen     = TRUE;
        // pInstanceData->bIsEof = FALSE;
        if( nAccessMode & GENERIC_WRITE )
            bIsWritable = TRUE;
    }
    SetErrorMode( nOldErrorMode );
}

/*************************************************************************
|*
|*    SvFileStream::ReOpen()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 15.06.94
|*    Letzte Aenderung  OV 15.06.94
|*
*************************************************************************/

void SvFileStream::ReOpen()
{
    if( !bIsOpen && aFilename.Len() )
        Open( aFilename, eStreamMode );
}

/*************************************************************************
|*
|*    SvFileStream::Close()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 15.06.94
|*    Letzte Aenderung  TPF 15.07.98
|*
*************************************************************************/

void SvFileStream::Close()
{
    if( IsOpen() )
    {
        if( nLockCounter )
        {
            nLockCounter = 1;
            UnlockFile();
        }
        Flush();
        CloseHandle( pInstanceData->hFile );
    }
    bIsOpen     = FALSE;
    nLockCounter= 0;
    bIsWritable = FALSE;
    SvStream::ClearBuffer();
    SvStream::ClearError();
}

/*************************************************************************
|*
|*    SvFileStream::ResetError()
|*
|*    Beschreibung      STREAM.SDW; Setzt Filepointer auf Dateianfang
|*    Ersterstellung    OV 15.06.94
|*    Letzte Aenderung  OV 15.06.94
|*
*************************************************************************/

void SvFileStream::ResetError()
{
    SvStream::ClearError();
}

/*************************************************************************
|*
|*    SvFileStream::SetSize()
|*
|*    Beschreibung      STREAM.SDW
|*    Ersterstellung    OV 19.10.95
|*    Letzte Aenderung  TPF 15.07.98
|*
*************************************************************************/

void SvFileStream::SetSize( ULONG nSize )
{

    if( IsOpen() )
    {
        int bError = FALSE;
        HANDLE hFile = pInstanceData->hFile;
        ULONG nOld = SetFilePointer( hFile, 0L, NULL, FILE_CURRENT );
        if( nOld != 0xffffffff )
        {
            if( SetFilePointer(hFile,nSize,NULL,FILE_BEGIN ) != 0xffffffff)
            {
                bool bSucc = SetEndOfFile( hFile );
                if( !bSucc )
                    bError = TRUE;
            }
            if( SetFilePointer( hFile,nOld,NULL,FILE_BEGIN ) == 0xffffffff)
                bError = TRUE;
        }
        if( bError )
            SetError(::GetSvError( GetLastError() ) );
    }
}

