/*************************************************************************
 *
 *  $RCSfile: methods.cxx,v $
 *
 *  $Revision: 1.50 $
 *
 *  last change: $Author: vg $ $Date: 2003-04-24 14:02:01 $
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


#ifndef _DATE_HXX //autogen
#include <tools/date.hxx>
#endif
#ifndef _SBXVAR_HXX
#include <svtools/sbxvar.hxx>
#endif
#ifndef _INTN_HXX //autogen
#include <tools/intn.hxx>
#endif
#ifndef _VOS_PROCESS_HXX
#include <vos/process.hxx>
#endif
#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef _SV_SETTINGS_HXX
#include <vcl/settings.hxx>
#endif
#ifndef _SV_SOUND_HXX //autogen
#include <vcl/sound.hxx>
#endif
#ifndef _SV_WINTYPES_HXX //autogen
#include <vcl/wintypes.hxx>
#endif
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SBXCLASS_HXX //autogen
#include <svtools/sbx.hxx>
#endif
#ifndef _ZFORLIST_HXX //autogen
#include <svtools/zforlist.hxx>
#endif
#include <rtl/math.hxx>
#include <tools/urlobj.hxx>
#include <osl/time.h>
#include <unotools/charclass.hxx>
#include <unotools/ucbstreamhelper.hxx>
#include <tools/isolang.hxx>

#ifdef OS2
#define INCL_WINWINDOWMGR
#define INCL_DOS
#endif

#if defined (WNT)
#ifndef _SVWIN_H
#include <tools/svwin.h>
#endif
#endif
#if defined (OS2)
#ifndef _SVPM_H
#include <tools/svpm.h>
#endif
#endif

#pragma hdrstop
#include "runtime.hxx"
#include "sbunoobj.hxx"
#ifdef _OLD_FILE_IMPL
#ifndef _FSYS_HXX //autogen
#include <tools/fsys.hxx>
#endif
#else
#include <osl/file.hxx>
#endif

#ifdef _USE_UNO
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/Locale.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess3.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/io/XStream.hpp>
#include <com/sun/star/io/XSeekable.hpp>

#ifndef PRODUCT
#include <com/sun/star/io/XTextOutputStream.hpp>
#include <com/sun/star/io/XActiveDataSource.hpp>
#endif

using namespace comphelper;
using namespace rtl;
using namespace osl;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::ucb;
using namespace com::sun::star::io;

#endif /* _USE_UNO */

//#define _ENABLE_CUR_DIR

#include "stdobj.hxx"
#include "stdobj1.hxx"
#include "rtlproto.hxx"
#include "basrid.hxx"
#include "sb.hrc"
#ifndef _SBIOSYS_HXX
#include "iosys.hxx"
#endif
#ifndef _DDECTRL_HXX
#include "ddectrl.hxx"
#endif
#include <sbintern.hxx>

#include <list>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#if defined (WIN) || defined (WNT) || defined (OS2)
#include <direct.h>   // _getdcwd get current work directory, _chdrive
#endif

#ifdef WIN
#include <dos.h>      // _dos_getfileattr
#include <errno.h>
#endif

#ifdef UNX
#include <errno.h>
#include <unistd.h>
#endif

#ifdef WNT
#include <io.h>
#endif

#ifdef MAC
#include <mac_start.h>

#ifndef __FILES__
  #include <Files.h>
#endif

#ifndef __ERRORS__
  #include <Errors.h>
#endif

#include <MAC_TOOLS.hxx>
#include <mac_end.h>
#endif

#if defined (OS2) && defined (__BORLANDC__)
#pragma option -w-par
#endif


static void FilterWhiteSpace( String& rStr )
{
    rStr.EraseAllChars( ' ' );
    rStr.EraseAllChars( '\t' );
    rStr.EraseAllChars( '\n' );
    rStr.EraseAllChars( '\r' );
}

static long GetDayDiff( const Date& rDate )
{
    Date aRefDate( 1,1,1900 );
    long nDiffDays;
    if ( aRefDate > rDate )
    {
        nDiffDays = (long)(aRefDate - rDate);
        nDiffDays *= -1;
    }
    else
        nDiffDays = (long)(rDate - aRefDate);
    nDiffDays += 2; // Anpassung VisualBasic: 1.Jan.1900 == 2
    return nDiffDays;
}

static CharClass& GetCharClass( void )
{
    static sal_Bool bNeedsInit = sal_True;
    static ::com::sun::star::lang::Locale aLocale;
    if( bNeedsInit )
    {
        bNeedsInit = sal_False;
        aLocale = Application::GetSettings().GetLocale();
    }
    static CharClass aCharClass( aLocale );
    return aCharClass;
}

static inline BOOL isFolder( FileStatus::Type aType )
{
    return ( aType == FileStatus::Directory || aType == FileStatus::Volume );
}


//*** UCB file access ***

// Converts possibly relative paths to absolute paths
// according to the setting done by ChDir/ChDrive
String getFullPath( const String& aRelPath )
{
    OUString aFileURL;

    // #80204 Try first if it already is a valid URL
    INetURLObject aURLObj( aRelPath );
    aFileURL = aURLObj.GetMainURL( INetURLObject::NO_DECODE );

    if( !aFileURL.getLength() )
    {
        File::getFileURLFromSystemPath( aRelPath, aFileURL );
    }

    return aFileURL;
}

// Sets (virtual) current path for UCB file access
void implChDir( const String& aDir )
{
    // TODO
}

// Sets (virtual) current drive for UCB file access
void implChDrive( const String& aDrive )
{
    // TODO
}

// Returns (virtual) current path for UCB file access
String implGetCurDir( void )
{
    String aRetStr;

    return aRetStr;
}

// TODO: -> SbiGlobals
static Reference< XSimpleFileAccess3 > getFileAccess( void )
{
    static Reference< XSimpleFileAccess3 > xSFI;
    if( !xSFI.is() )
    {
        Reference< XMultiServiceFactory > xSMgr = getProcessServiceFactory();
        if( xSMgr.is() )
        {
            xSFI = Reference< XSimpleFileAccess3 >( xSMgr->createInstance
                ( OUString::createFromAscii( "com.sun.star.ucb.SimpleFileAccess" ) ), UNO_QUERY );
        }
    }
    return xSFI;
}



// Properties und Methoden legen beim Get (bPut = FALSE) den Returnwert
// im Element 0 des Argv ab; beim Put (bPut = TRUE) wird der Wert aus
// Element 0 gespeichert.

// CreateObject( class )

RTLFUNC(CreateObject)
{
    String aClass( rPar.Get( 1 )->GetString() );
    SbxObjectRef p = SbxBase::CreateObject( aClass );
    if( !p )
        StarBASIC::Error( SbERR_CANNOT_LOAD );
    else
    {
        // Convenience: BASIC als Parent eintragen
        p->SetParent( pBasic );
        rPar.Get( 0 )->PutObject( p );
    }
}

// Error( n )

RTLFUNC(Error)
{
    if( !pBasic )
        StarBASIC::Error( SbERR_INTERNAL_ERROR );
    else
    {
        String aErrorMsg;
        SbError nErr = 0L;
        if( rPar.Count() == 1 )
        {
            nErr = StarBASIC::GetErr();
            aErrorMsg = StarBASIC::GetErrorMsg();
        }
        else
        {
            INT32 nCode = rPar.Get( 1 )->GetLong();
            if( nCode > 65535L )
                StarBASIC::Error( SbERR_CONVERSION );
            else
                nErr = StarBASIC::GetSfxFromVBError( (USHORT)nCode );
        }
        pBasic->MakeErrorText( nErr, aErrorMsg );
        rPar.Get( 0 )->PutString( pBasic->GetErrorText() );
    }
}

// Sinus

RTLFUNC(Sin)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbxVariableRef pArg = rPar.Get( 1 );
        rPar.Get( 0 )->PutDouble( sin( pArg->GetDouble() ) );
    }
}

// Cosinus

RTLFUNC(Cos)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbxVariableRef pArg = rPar.Get( 1 );
        rPar.Get( 0 )->PutDouble( cos( pArg->GetDouble() ) );
    }
}

// Atn

RTLFUNC(Atn)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbxVariableRef pArg = rPar.Get( 1 );
        rPar.Get( 0 )->PutDouble( atan( pArg->GetDouble() ) );
    }
}



RTLFUNC(Abs)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbxVariableRef pArg = rPar.Get( 1 );
        rPar.Get( 0 )->PutDouble( fabs( pArg->GetDouble() ) );
    }
}


RTLFUNC(Asc)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbxVariableRef pArg = rPar.Get( 1 );
        String aStr( pArg->GetString() );
        if ( aStr.Len() == 0 )
        {
            StarBASIC::Error( SbERR_BAD_ARGUMENT );
            rPar.Get(0)->PutEmpty();
        }
        else
        {
            sal_Unicode aCh = aStr.GetBuffer()[0];
            rPar.Get(0)->PutInteger( (INT16)aCh );
        }
    }
}

RTLFUNC(Chr)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbxVariableRef pArg = rPar.Get( 1 );
        sal_Unicode aCh = (sal_Unicode)pArg->GetUShort();
        String aStr( aCh );
        rPar.Get(0)->PutString( aStr );
    }
}


#ifdef UNX
#define _MAX_PATH 260
#define _PATH_INCR 250
#endif

RTLFUNC(CurDir)
{
    // #57064 Obwohl diese Funktion nicht mit DirEntry arbeitet, ist sie von
    // der Anpassung an virtuelle URLs nich betroffen, da bei Nutzung der
    // DirEntry-Funktionalitaet keine Moeglichkeit besteht, das aktuelle so
    // zu ermitteln, dass eine virtuelle URL geliefert werden koennte.

//  rPar.Get(0)->PutEmpty();
#if defined (WIN) || defined (WNT) || (defined (OS2) && !defined( WTC ))
    int nCurDir = 0;  // Current dir // JSM
    if ( rPar.Count() == 2 )
    {
        String aDrive = rPar.Get(1)->GetString();
        if ( aDrive.Len() != 1 )
        {
            StarBASIC::Error( SbERR_BAD_ARGUMENT );
            return;
        }
        else
        {
            nCurDir = (int)aDrive.GetBuffer()[0];
            if ( !isalpha( nCurDir ) )
            {
                StarBASIC::Error( SbERR_BAD_ARGUMENT );
                return;
            }
            else
                nCurDir -= ( 'A' - 1 );
        }
    }
    char* pBuffer = new char[ _MAX_PATH ];
#ifdef MTW
    int old = _getdrive();
    _chdrive(nCurDir);

    if ( getcwd( pBuffer, _MAX_PATH ) != 0 )
        rPar.Get(0)->PutString( String::CreateFromAscii( pBuffer ) );
    else
        StarBASIC::Error( SbERR_NO_DEVICE );
    delete pBuffer;
    _chdrive(old);
#else
#ifdef OS2
    if( !nCurDir )
        nCurDir = _getdrive();
#endif
    if ( _getdcwd( nCurDir, pBuffer, _MAX_PATH ) != 0 )
        rPar.Get(0)->PutString( String::CreateFromAscii( pBuffer ) );
    else
        StarBASIC::Error( SbERR_NO_DEVICE );
    delete pBuffer;
#endif

#elif defined MAC

    Str255              aBuffer;
    FSSpec              aFileSpec;      // Pseudofile
    String              aPar1;
    OSErr               nErr;

    // Erstmal aktuelle Pfad bestimmen
    nErr = FSMakeFSSpec(0,0,"\p:X",&aFileSpec);

    PathNameFromDirID( aFileSpec.parID,aFileSpec.vRefNum, (char*) aBuffer);
    String aPath((char*) &aBuffer[1],aBuffer[0]);

    if ( rPar.Count() == 2 )
    {
        aPar1 = rPar.Get(1)->GetString();

        // Wen kein ':' drin ist dann haengen wir (netterweise) einen an
        if (aPar1.Search(':') == STRING_NOTFOUND)
            aPar1 += ':';
        USHORT nFirstColon = aPar1.Search(':');
        if (!aPar1.Len() ||
            nFirstColon != (aPar1.Len() - 1))
            // Kein ':' am Ende oder mehr als ein ':' oder leerer String
        {
            StarBASIC::Error( SbERR_BAD_ARGUMENT );
            return;
        }
        // Is Param1 eventuelle das Volume des aktuellen Pfades ?
        USHORT nMatchPoint = aPath.Match(aPar1);
        if (nMatchPoint != (nFirstColon + 1))
        {
            String aPseudoFile(aPar1);
            aPseudoFile += 'X'; // Pseudodatei

            nErr = FSMakeFSSpec(0,0,aPseudoFile.GetPascalStr(),&aFileSpec);

            if(nErr == nsvErr)
            {
                StarBASIC::Error( SbERR_NO_DEVICE );
                return;
            }
            aPath = aPar1;
        }
    }

    rPar.Get(0)->PutString(aPath);

#elif defined( UNX )

    int nSize = _PATH_INCR;
    char* pMem;
    while( TRUE )
      {
        pMem = new char[nSize];
        if( !pMem )
          {
            StarBASIC::Error( SbERR_NO_MEMORY );
            return;
          }
        if( getcwd( pMem, nSize-1 ) != NULL )
          {
            rPar.Get(0)->PutString( String::CreateFromAscii(pMem) );
            delete pMem;
            return;
          }
        if( errno != ERANGE )
          {
            StarBASIC::Error( SbERR_INTERNAL_ERROR );
            delete pMem;
            return;
          }
        delete pMem;
        nSize += _PATH_INCR;
      };

#endif
}

RTLFUNC(ChDir) // JSM
{
    rPar.Get(0)->PutEmpty();
    if (rPar.Count() == 2)
    {
#ifdef _ENABLE_CUR_DIR
        String aPath = rPar.Get(1)->GetString();
        BOOL bError = FALSE;
#ifdef WNT
        // #55997 Laut MI hilft es bei File-URLs einen DirEntry zwischenzuschalten
        // #40996 Harmoniert bei Verwendung der WIN32-Funktion nicht mit getdir
        DirEntry aEntry( aPath );
        ByteString aFullPath( aEntry.GetFull(), gsl_getSystemTextEncoding() );
        if( chdir( aFullPath.GetBuffer()) )
            bError = TRUE;
#else
        if (!DirEntry(aPath).SetCWD())
            bError = TRUE;
#endif
        if( bError )
            StarBASIC::Error( SbERR_PATH_NOT_FOUND );
#endif
    }
    else
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
}

RTLFUNC(ChDrive) // JSM
{
    rPar.Get(0)->PutEmpty();
    if (rPar.Count() == 2)
    {
#ifdef _ENABLE_CUR_DIR
        // Keine Laufwerke in Unix
#ifndef UNX
        String aPar1 = rPar.Get(1)->GetString();

#if defined (WIN) || defined (WNT) || (defined (OS2) && !defined (WTC))
        if (aPar1.Len() > 0)
        {
            int nCurDrive = (int)aPar1.GetBuffer()[0]; ;
            if ( !isalpha( nCurDrive ) )
            {
                StarBASIC::Error( SbERR_BAD_ARGUMENT );
                return;
            }
            else
                nCurDrive -= ( 'A' - 1 );
            if (_chdrive(nCurDrive))
                StarBASIC::Error( SbERR_NO_DEVICE );
        }
#elif defined MAC
        // Wen kein ':' drin ist dann haengen wir (netterweise) einen an
        if (aPar1.Search(':') == STRING_NOTFOUND)
            aPar1 += ':';
        if (!aPar1.Len() ||
            aPar1.Search(':') != (aPar1.Len() - 1))
            // Kein ':' am Ende oder mehr als ein ':' oder leerer String
        {
            StarBASIC::Error( SbERR_BAD_ARGUMENT );
            return;
        }

        DirEntry aDrive(aPar1);
        if (aDrive.SetCWD())
            return;
        else
            StarBASIC::Error( SbERR_NO_DEVICE );
#endif

#endif
        // #ifndef UNX
#endif
    }
    else
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
}


// Implementation of StepRENAME with UCB
void implStepRenameUCB( const String& aSource, const String& aDest )
{
    Reference< XSimpleFileAccess3 > xSFI = getFileAccess();
    if( xSFI.is() )
    {
        try
        {
            xSFI->move( getFullPath( aSource ), getFullPath( aDest ) );
        }
        catch( Exception & )
        {
            StarBASIC::Error( ERRCODE_IO_GENERAL );
        }
    }
}

// Implementation of StepRENAME with OSL
void implStepRenameOSL( const String& aSource, const String& aDest )
{
    FileBase::RC nRet = File::move( getFullPathUNC( aSource ), getFullPathUNC( aDest ) );
    if( nRet != FileBase::E_None )
    {
        StarBASIC::Error( SbERR_PATH_NOT_FOUND );
    }
}

RTLFUNC(FileCopy) // JSM
{
    rPar.Get(0)->PutEmpty();
    if (rPar.Count() == 3)
    {
        String aSource = rPar.Get(1)->GetString();
        String aDest = rPar.Get(2)->GetString();
        // <-- UCB
        if( hasUno() )
        {
            Reference< XSimpleFileAccess3 > xSFI = getFileAccess();
            if( xSFI.is() )
            {
                try
                {
                    xSFI->copy( getFullPath( aSource ), getFullPath( aDest ) );
                }
                catch( Exception & )
                {
                    StarBASIC::Error( ERRCODE_IO_GENERAL );
                }
            }
        }
        else
        // --> UCB
        {
#ifdef _OLD_FILE_IMPL
            DirEntry aSourceDirEntry(aSource);
            if (aSourceDirEntry.Exists())
            {
                if (aSourceDirEntry.CopyTo(DirEntry(aDest),FSYS_ACTION_COPYFILE) != FSYS_ERR_OK)
                    StarBASIC::Error( SbERR_PATH_NOT_FOUND );
            }
            else
                    StarBASIC::Error( SbERR_PATH_NOT_FOUND );
#else
            FileBase::RC nRet = File::copy( getFullPathUNC( aSource ), getFullPathUNC( aDest ) );
            if( nRet != FileBase::E_None )
            {
                StarBASIC::Error( SbERR_PATH_NOT_FOUND );
            }
#endif
        }
    }
    else
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
}

RTLFUNC(Kill) // JSM
{
    rPar.Get(0)->PutEmpty();
    if (rPar.Count() == 2)
    {
        String aFileSpec = rPar.Get(1)->GetString();

        // <-- UCB
        if( hasUno() )
        {
            Reference< XSimpleFileAccess3 > xSFI = getFileAccess();
            if( xSFI.is() )
            {
                try
                {
                    xSFI->kill( getFullPath( aFileSpec ) );
                }
                catch( Exception & )
                {
                    StarBASIC::Error( ERRCODE_IO_GENERAL );
                }
            }
        }
        else
        // --> UCB
        {
#ifdef _OLD_FILE_IMPL
            if(DirEntry(aFileSpec).Kill() != FSYS_ERR_OK)
                StarBASIC::Error( SbERR_PATH_NOT_FOUND );
#else
            File::remove( getFullPathUNC( aFileSpec ) );
#endif
        }
    }
    else
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
}

RTLFUNC(MkDir) // JSM
{
    rPar.Get(0)->PutEmpty();
    if (rPar.Count() == 2)
    {
        String aPath = rPar.Get(1)->GetString();

        // <-- UCB
        if( hasUno() )
        {
            Reference< XSimpleFileAccess3 > xSFI = getFileAccess();
            if( xSFI.is() )
            {
                try
                {
                    xSFI->createFolder( getFullPath( aPath ) );
                }
                catch( Exception & )
                {
                    StarBASIC::Error( ERRCODE_IO_GENERAL );
                }
            }
        }
        else
        // --> UCB
        {
#ifdef _OLD_FILE_IMPL
            if (!DirEntry(aPath).MakeDir())
                StarBASIC::Error( SbERR_PATH_NOT_FOUND );
#else
            Directory::create( getFullPathUNC( aPath ) );
#endif
        }
    }
    else
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
}


#ifndef _OLD_FILE_IMPL

// In OSL only empty directories can be deleted
// so we have to delete all files recursively
void implRemoveDirRecursive( const String& aDirPath )
{
    DirectoryItem aItem;
    FileBase::RC nRet = DirectoryItem::get( aDirPath, aItem );
    sal_Bool bExists = (nRet == FileBase::E_None);

    FileStatus aFileStatus( FileStatusMask_Type );
    nRet = aItem.getFileStatus( aFileStatus );
    FileStatus::Type aType = aFileStatus.getFileType();
    sal_Bool bFolder = isFolder( aType );

    if( !bExists || !bFolder )
    {
        StarBASIC::Error( SbERR_PATH_NOT_FOUND );
        return;
    }

    Directory aDir( aDirPath );
    nRet = aDir.open();
    if( nRet != FileBase::E_None )
    {
        StarBASIC::Error( SbERR_PATH_NOT_FOUND );
        return;
    }

    for( ;; )
    {
        DirectoryItem aItem;
        nRet = aDir.getNextItem( aItem );
        if( nRet != FileBase::E_None )
            break;

        // Handle flags
        FileStatus aFileStatus( FileStatusMask_Type | FileStatusMask_FileURL );
        nRet = aItem.getFileStatus( aFileStatus );
        OUString aPath = aFileStatus.getFileURL();

        // Directory?
        FileStatus::Type aType = aFileStatus.getFileType();
        sal_Bool bFolder = isFolder( aType );
        if( bFolder )
        {
            implRemoveDirRecursive( aPath );
        }
        else
        {
            File::remove( aPath );
        }
    }
    nRet = aDir.close();

    nRet = Directory::remove( aDirPath );
}
#endif


RTLFUNC(RmDir) // JSM
{
    rPar.Get(0)->PutEmpty();
    if (rPar.Count() == 2)
    {
        String aPath = rPar.Get(1)->GetString();
        // <-- UCB
        if( hasUno() )
        {
            Reference< XSimpleFileAccess3 > xSFI = getFileAccess();
            if( xSFI.is() )
            {
                try
                {
                    xSFI->kill( getFullPath( aPath ) );
                }
                catch( Exception & )
                {
                    StarBASIC::Error( ERRCODE_IO_GENERAL );
                }
            }
        }
        else
        // --> UCB
        {
#ifdef _OLD_FILE_IMPL
            DirEntry aDirEntry(aPath);
            if (aDirEntry.Kill() != FSYS_ERR_OK)
                StarBASIC::Error( SbERR_PATH_NOT_FOUND );
#else
            implRemoveDirRecursive( getFullPathUNC( aPath ) );
#endif
        }
    }
    else
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
}

RTLFUNC(SendKeys) // JSM
{
    rPar.Get(0)->PutEmpty();
    StarBASIC::Error(SbERR_NOT_IMPLEMENTED);
}

RTLFUNC(Exp)
{
    ULONG nArgCount = rPar.Count();
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        double aDouble = rPar.Get( 1 )->GetDouble();
        aDouble = exp( aDouble );
        checkArithmeticOverflow( aDouble );
        rPar.Get( 0 )->PutDouble( aDouble );
    }
}

RTLFUNC(FileLen)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbxVariableRef pArg = rPar.Get( 1 );
        String aStr( pArg->GetString() );
        INT32 nLen = 0;
        // <-- UCB
        if( hasUno() )
        {
            Reference< XSimpleFileAccess3 > xSFI = getFileAccess();
            if( xSFI.is() )
            {
                try
                {
                    nLen = xSFI->getSize( getFullPath( aStr ) );
                }
                catch( Exception & )
                {
                    StarBASIC::Error( ERRCODE_IO_GENERAL );
                }
            }
        }
        else
        // --> UCB
        {
#ifdef _OLD_FILE_IMPL
            FileStat aStat = DirEntry( aStr );
            nLen = aStat.GetSize();
#else
            DirectoryItem aItem;
            FileBase::RC nRet = DirectoryItem::get( getFullPathUNC( aStr ), aItem );
            FileStatus aFileStatus( FileStatusMask_FileSize );
            nRet = aItem.getFileStatus( aFileStatus );
            nLen = (INT32)aFileStatus.getFileSize();
#endif
        }
        rPar.Get(0)->PutLong( (long)nLen );
    }
}


RTLFUNC(Hex)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        char aBuffer[16];
        SbxVariableRef pArg = rPar.Get( 1 );
        if ( pArg->IsInteger() )
            snprintf( aBuffer, sizeof(aBuffer), "%X", pArg->GetInteger() );
        else
            snprintf( aBuffer, sizeof(aBuffer), "%lX", pArg->GetLong() );
        rPar.Get(0)->PutString( String::CreateFromAscii( aBuffer ) );
    }
}

// InStr( [start],string,string,[compare] )

RTLFUNC(InStr)
{
    ULONG nArgCount = rPar.Count()-1;
    if ( nArgCount < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        USHORT nStartPos = 1;

        USHORT nFirstStringPos = 1;
        if ( nArgCount >= 3 )
        {
            nStartPos = (USHORT)(rPar.Get(1)->GetInteger());
            if ( nStartPos == 0 )
            {
                StarBASIC::Error( SbERR_BAD_ARGUMENT );
                nStartPos = 1;
            }
            nFirstStringPos++;
        }
        int bNotCaseSensitive = 1;  // wird noch nicht ausgewertet
        if ( nArgCount == 4 )
            bNotCaseSensitive = rPar.Get(4)->GetInteger();

        USHORT nPos;
        const String& rToken = rPar.Get(nFirstStringPos+1)->GetString();

        // #97545 Always find empty string
        if( !rToken.Len() )
        {
            nPos = nStartPos;
        }
        else
        {
            if( !bNotCaseSensitive )
            {
                const String& rStr1 = rPar.Get(nFirstStringPos)->GetString();

                nPos = rStr1.Search( rToken, nStartPos-1 );
                if ( nPos == STRING_NOTFOUND )
                    nPos = 0;
                else
                    nPos++;
            }
            else
            {
                String aStr1 = rPar.Get(nFirstStringPos)->GetString();
                String aToken = rToken;

                aStr1.ToUpperAscii();
                aToken.ToUpperAscii();

                nPos = aStr1.Search( aToken, nStartPos-1 );
                if ( nPos == STRING_NOTFOUND )
                    nPos = 0;
                else
                    nPos++;
            }
        }
        rPar.Get(0)->PutInteger( (int)nPos );
    }
}


/*
    Int( 2.8 )  =  2.0
    Int( -2.8 ) = -3.0
    Fix( 2.8 )  =  2.0
    Fix( -2.8 ) = -2.0    <- !!
*/

RTLFUNC(Int)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbxVariableRef pArg = rPar.Get( 1 );
        double aDouble= pArg->GetDouble();
        /*
            floor( 2.8 ) =  2.0
            floor( -2.8 ) = -3.0
        */
        aDouble = floor( aDouble );
        rPar.Get(0)->PutDouble( aDouble );
    }
}



RTLFUNC(Fix)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbxVariableRef pArg = rPar.Get( 1 );
        double aDouble = pArg->GetDouble();
        if ( aDouble >= 0.0 )
            aDouble = floor( aDouble );
        else
            aDouble = ceil( aDouble );
        rPar.Get(0)->PutDouble( aDouble );
    }
}


RTLFUNC(LCase)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        CharClass& rCharClass = GetCharClass();
        String aStr( rPar.Get(1)->GetString() );
        rCharClass.toLower( aStr );
        rPar.Get(0)->PutString( aStr );
    }
}

RTLFUNC(Left)
{
    ULONG nArgCount = rPar.Count();
    if ( rPar.Count() < 3 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        String aStr( rPar.Get(1)->GetString() );
        short nCount = (USHORT)( rPar.Get(2)->GetLong() );
        if ( nCount < 0 )
            StarBASIC::Error( SbERR_BAD_ARGUMENT );
        else
        {
            aStr.Erase( (USHORT)nCount );
            rPar.Get(0)->PutString( aStr );
        }
    }
}

RTLFUNC(Log)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        double aArg = rPar.Get(1)->GetDouble();
        if ( aArg > 0 )
        {
            double d = log( aArg );
            checkArithmeticOverflow( d );
            rPar.Get( 0 )->PutDouble( d );
        }
        else
            StarBASIC::Error( SbERR_BAD_ARGUMENT );
    }
}

RTLFUNC(LTrim)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        String aStr( rPar.Get(1)->GetString() );
        aStr.EraseLeadingChars();
        rPar.Get(0)->PutString( aStr );
    }
}


// Mid( String, nStart, nLength )

RTLFUNC(Mid)
{
    ULONG nArgCount = rPar.Count()-1;
    if ( nArgCount < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        // #23178: Funktionalitaet von Mid$ als Anweisung nachbilden, indem
        // als weiterer (4.) Parameter ein Ersetzungsstring aufgenommen wird.
        // Anders als im Original kann in dieser Variante der 3. Parameter
        // nLength nicht weggelassen werden. Ist ueber bWrite schon vorgesehen.
        if( nArgCount == 4 )
            bWrite = TRUE;

        String aArgStr = rPar.Get(1)->GetString();
        USHORT nStartPos = (USHORT)(rPar.Get(2)->GetLong() );
        if ( nStartPos == 0 )
            StarBASIC::Error( SbERR_BAD_ARGUMENT );
        else
        {
            nStartPos--;
            USHORT nLen = 0xffff;
            if ( nArgCount == 3 || bWrite )
                nLen = (USHORT)(rPar.Get(3)->GetLong() );
            String aResultStr;
            if ( bWrite )
            {
                aResultStr = aArgStr;
                aResultStr.Erase( nStartPos, nLen );
                aResultStr.Insert(rPar.Get(4)->GetString(),0,nLen,nStartPos);
                rPar.Get(1)->PutString( aResultStr );
            }
            else
            {
                aResultStr = aArgStr.Copy( nStartPos, nLen );
                rPar.Get(0)->PutString( aResultStr );
            }
        }
    }
}

RTLFUNC(Oct)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        char aBuffer[16];
        SbxVariableRef pArg = rPar.Get( 1 );
        if ( pArg->IsInteger() )
            snprintf( aBuffer, sizeof(aBuffer), "%o", pArg->GetInteger() );
        else
            snprintf( aBuffer, sizeof(aBuffer), "%lo", pArg->GetLong() );
        rPar.Get(0)->PutString( String::CreateFromAscii( aBuffer ) );
    }
}

RTLFUNC(Right)
{
    ULONG nArgCount = rPar.Count();
    if ( rPar.Count() < 3 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        const String& rStr = rPar.Get(1)->GetString();
        USHORT nResultLen = (USHORT)(rPar.Get(2)->GetLong() );
        USHORT nStrLen = rStr.Len();
        if ( nResultLen > nStrLen )
            nResultLen = nStrLen;
        String aResultStr = rStr.Copy( nStrLen-nResultLen );
        rPar.Get(0)->PutString( aResultStr );
    }
}

RTLFUNC(RTrim)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        String aStr( rPar.Get(1)->GetString() );
        aStr.EraseTrailingChars();
        rPar.Get(0)->PutString( aStr );
    }
}

RTLFUNC(Sgn)
{
    ULONG nArgCount = rPar.Count();
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        double aDouble = rPar.Get(1)->GetDouble();
        INT16 nResult = 0;
        if ( aDouble > 0 )
            nResult = 1;
        else if ( aDouble < 0 )
            nResult = -1;
        rPar.Get(0)->PutInteger( nResult );
    }
}

RTLFUNC(Space)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        String aStr;
        aStr.Fill( (USHORT)(rPar.Get(1)->GetLong() ));
        rPar.Get(0)->PutString( aStr );
    }
}

RTLFUNC(Spc)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        String aStr;
        aStr.Fill( (USHORT)(rPar.Get(1)->GetLong() ));
        rPar.Get(0)->PutString( aStr );
    }
}

RTLFUNC(Sqr)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        double aDouble = rPar.Get(1)->GetDouble();
        if ( aDouble >= 0 )
            rPar.Get(0)->PutDouble( sqrt( aDouble ));
        else
            StarBASIC::Error( SbERR_BAD_ARGUMENT );
    }
}

RTLFUNC(Str)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        String aStr;
        rPar.Get( 1 )->Format( aStr );
        // Numbers start with a space
        if( rPar.Get( 1 )->IsNumericRTL() )
        {
            aStr.Insert( ' ', 0 );

            // Kommas durch Punkte ersetzen, damit es symmetrisch zu Val ist!
            aStr.SearchAndReplace( ',', '.' );
        }
        rPar.Get(0)->PutString( aStr );
    }
}

RTLFUNC(StrComp)
{
    if ( rPar.Count() < 3 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        rPar.Get(0)->PutEmpty();
        return;
    }
    const String& rStr1 = rPar.Get(1)->GetString();
    const String& rStr2 = rPar.Get(2)->GetString();
    INT16 nIgnoreCase = TRUE;
    if ( rPar.Count() == 4 )
        nIgnoreCase = rPar.Get(3)->GetInteger();

    StringCompare aResult;
    int nRetValue = 0;
    if ( !nIgnoreCase )
    {
        ::utl::TransliterationWrapper* pTransliterationWrapper = GetSbData()->pTransliterationWrapper;
        if( !pTransliterationWrapper )
        {
            Reference< XMultiServiceFactory > xSMgr = getProcessServiceFactory();
            pTransliterationWrapper = GetSbData()->pTransliterationWrapper =
                new ::utl::TransliterationWrapper( xSMgr,
                    ::com::sun::star::i18n::TransliterationModules_IGNORE_CASE |
                    ::com::sun::star::i18n::TransliterationModules_IGNORE_KANA |
                    ::com::sun::star::i18n::TransliterationModules_IGNORE_WIDTH );
        }

        LanguageType eLangType = GetpApp()->GetSettings().GetLanguage();
        pTransliterationWrapper->loadModuleIfNeeded( eLangType );
        nRetValue = pTransliterationWrapper->compareString( rStr1, rStr2 );
    }
    else
    {
        aResult = rStr1.CompareTo( rStr2 );
        if ( aResult == COMPARE_LESS )
            nRetValue = -1;
        else if ( aResult == COMPARE_GREATER )
            nRetValue = 1;
    }

    rPar.Get(0)->PutInteger( nRetValue );
}

RTLFUNC(String)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        String aStr;
        sal_Unicode aFiller;
        USHORT nCount = (USHORT)(rPar.Get(1)->GetLong());
        if( rPar.Get(2)->GetType() == SbxINTEGER )
            aFiller = (char)rPar.Get(2)->GetInteger();
        else
        {
            const String& rStr = rPar.Get(2)->GetString();
            aFiller = rStr.GetBuffer()[0];
        }
        aStr.Fill( nCount, aFiller );
        rPar.Get(0)->PutString( aStr );
    }
}

RTLFUNC(Tan)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbxVariableRef pArg = rPar.Get( 1 );
        rPar.Get( 0 )->PutDouble( tan( pArg->GetDouble() ) );
    }
}

RTLFUNC(UCase)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        CharClass& rCharClass = GetCharClass();
        String aStr( rPar.Get(1)->GetString() );
        rCharClass.toUpper( aStr );
        rPar.Get(0)->PutString( aStr );
    }
}


RTLFUNC(Val)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        double nResult;
        char* pEndPtr;

        String aStr( rPar.Get(1)->GetString() );
// lt. Mikkysoft bei Kommas abbrechen!
//      for( USHORT n=0; n < aStr.Len(); n++ )
//          if( aStr[n] == ',' ) aStr[n] = '.';

        FilterWhiteSpace( aStr );
        if ( aStr.GetBuffer()[0] == '&' && aStr.Len() > 1 )
        {
            int nRadix = 10;
            char aChar = (char)aStr.GetBuffer()[1];
            if ( aChar == 'h' || aChar == 'H' )
                nRadix = 16;
            else if ( aChar == 'o' || aChar == 'O' )
                nRadix = 8;
            if ( nRadix != 10 )
            {
                ByteString aByteStr( aStr, gsl_getSystemTextEncoding() );
                INT16 nlResult = (INT16)strtol( aByteStr.GetBuffer()+2, &pEndPtr, nRadix);
                nResult = (double)nlResult;
            }
        }
        else
        {
            // #57844 Lokalisierte Funktion benutzen
            nResult = ::rtl::math::stringToDouble( aStr, '.', ',', NULL, NULL );
            checkArithmeticOverflow( nResult );
            // ATL: nResult = strtod( aStr.GetStr(), &pEndPtr );
        }

        rPar.Get(0)->PutDouble( nResult );
    }
}


// Helper functions for date conversion
INT16 implGetDateDay( double aDate )
{
    aDate -= 2.0; // normieren: 1.1.1900 => 0.0
    Date aRefDate( 1, 1, 1900 );
    if ( aDate >= 0.0 )
    {
        aDate = floor( aDate );
        aRefDate += (ULONG)aDate;
    }
    else
    {
        aDate = ceil( aDate );
        aRefDate -= (ULONG)(-1.0 * aDate);
    }

    INT16 nRet = (INT16)( aRefDate.GetDay() );
    return nRet;
}

INT16 implGetDateMonth( double aDate )
{
    Date aRefDate( 1,1,1900 );
    long nDays = (long)aDate;
    nDays -= 2; // normieren: 1.1.1900 => 0.0
    aRefDate += nDays;
    INT16 nRet = (INT16)( aRefDate.GetMonth() );
    return nRet;
}

INT16 implGetDateYear( double aDate )
{
    Date aRefDate( 1,1,1900 );
    long nDays = (long) aDate;
    nDays -= 2; // normieren: 1.1.1900 => 0.0
    aRefDate += nDays;
    INT16 nRet = (INT16)( aRefDate.GetYear() );
    return nRet;
}

BOOL implDateSerial( INT16 nYear, INT16 nMonth, INT16 nDay, double& rdRet )
{
    if ( nYear < 100 )
        nYear += 1900;
    if ((nYear < 100 || nYear > 9999)   ||
        (nMonth < 1 || nMonth > 12 )    ||
        (nDay < 1 || nDay > 31 ))
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return FALSE;
    }

    Date aCurDate( nDay, nMonth, nYear );
    long nDiffDays = GetDayDiff( aCurDate );
    rdRet = (double)nDiffDays;
    return TRUE;
}

// Function to convert date to ISO 8601 date format
RTLFUNC(CDateToIso)
{
    if ( rPar.Count() == 2 )
    {
        double aDate = rPar.Get(1)->GetDate();

        char Buffer[9];
        snprintf( Buffer, sizeof( Buffer ), "%04ld%02ld%02ld",
            implGetDateYear( aDate ),
            implGetDateMonth( aDate ),
            implGetDateDay( aDate ) );
        String aRetStr = String::CreateFromAscii( Buffer );
        rPar.Get(0)->PutString( aRetStr );
    }
    else
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
}

// Function to convert date from ISO 8601 date format
RTLFUNC(CDateFromIso)
{
    if ( rPar.Count() == 2 )
    {
        String aStr = rPar.Get(1)->GetString();
        INT16 iMonthStart = aStr.Len() - 4;
        String aYearStr  = aStr.Copy( 0, iMonthStart );
        String aMonthStr = aStr.Copy( iMonthStart, 2 );
        String aDayStr   = aStr.Copy( iMonthStart+2, 2 );

        double dDate;
        if( implDateSerial( (INT16)aYearStr.ToInt32(),
            (INT16)aMonthStr.ToInt32(), (INT16)aDayStr.ToInt32(), dDate ) )
        {
            rPar.Get(0)->PutDate( dDate );
        }
    }
    else
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
}

RTLFUNC(DateSerial)
{
    if ( rPar.Count() < 4 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }
    INT16 nYear = rPar.Get(1)->GetInteger();
    INT16 nMonth = rPar.Get(2)->GetInteger();
    INT16 nDay = rPar.Get(3)->GetInteger();

    double dDate;
    if( implDateSerial( nYear, nMonth, nDay, dDate ) )
        rPar.Get(0)->PutDate( dDate );
}

RTLFUNC(TimeSerial)
{
    if ( rPar.Count() < 4 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }
    INT16 nHour = rPar.Get(1)->GetInteger();
    if ( nHour == 24 )
        nHour = 0;                      // Wegen UNO DateTimes, die bis 24 Uhr gehen
    INT16 nMinute = rPar.Get(2)->GetInteger();
    INT16 nSecond = rPar.Get(3)->GetInteger();
    if ((nHour < 0 || nHour > 23)   ||
        (nMinute < 0 || nMinute > 59 )  ||
        (nSecond < 0 || nSecond > 59 ))
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }

    INT32 nSeconds = nHour;
    nSeconds *= 3600;
    nSeconds += nMinute * 60;
    nSeconds += nSecond;
    double nDays = ((double)nSeconds) / (double)(86400.0);
    rPar.Get(0)->PutDate( nDays ); // JSM
}

RTLFUNC(DateValue)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        // #39629 pINST pruefen, kann aus URL-Zeile gerufen werden
        SvNumberFormatter* pFormatter = NULL;
        if( pINST )
            pFormatter = pINST->GetNumberFormatter();
        else
        {
            ULONG n;    // Dummy
            SbiInstance::PrepareNumberFormatter( pFormatter, n, n, n );
        }

        ULONG nIndex;
        double fResult;
        String aStr( rPar.Get(1)->GetString() );
        BOOL bSuccess = pFormatter->IsNumberFormat( aStr, nIndex, fResult );
        short nType = pFormatter->GetType( nIndex );
        if(bSuccess && (nType==NUMBERFORMAT_DATE || nType==NUMBERFORMAT_DATETIME))
        {
            if ( nType == NUMBERFORMAT_DATETIME )
            {
                // Zeit abschneiden
                if ( fResult  > 0.0 )
                    fResult = floor( fResult );
                else
                    fResult = ceil( fResult );
            }
            // fResult += 2.0; // Anpassung  StarCalcFormatter
            rPar.Get(0)->PutDate( fResult ); // JSM
        }
        else
            StarBASIC::Error( SbERR_CONVERSION );

        // #39629 pFormatter kann selbst angefordert sein
        if( !pINST )
            delete pFormatter;
    }
}

RTLFUNC(TimeValue)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        // #39629 pINST pruefen, kann aus URL-Zeile gerufen werden
        SvNumberFormatter* pFormatter = NULL;
        if( pINST )
            pFormatter = pINST->GetNumberFormatter();
        else
        {
            ULONG n;    // Dummy
            SbiInstance::PrepareNumberFormatter( pFormatter, n, n, n );
        }

        ULONG nIndex;
        double fResult;
        BOOL bSuccess = pFormatter->IsNumberFormat( rPar.Get(1)->GetString(),
                                                   nIndex, fResult );
        short nType = pFormatter->GetType(nIndex);
        if(bSuccess && (nType==NUMBERFORMAT_TIME||nType==NUMBERFORMAT_DATETIME))
        {
            if ( nType == NUMBERFORMAT_DATETIME )
                // Tage abschneiden
                fResult = fmod( fResult, 1 );
            rPar.Get(0)->PutDate( fResult ); // JSM
        }
        else
            StarBASIC::Error( SbERR_CONVERSION );

        // #39629 pFormatter kann selbst angefordert sein
        if( !pINST )
            delete pFormatter;
    }
}

RTLFUNC(Day)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbxVariableRef pArg = rPar.Get( 1 );
        double aDate = pArg->GetDate();

        INT16 nDay = implGetDateDay( aDate );
        rPar.Get(0)->PutInteger( nDay );
    }
}

RTLFUNC(Weekday)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        Date aRefDate( 1,1,1900 );
        long nDays = (long) rPar.Get(1)->GetDate();
        nDays -= 2; // normieren: 1.1.1900 => 0
        aRefDate += nDays;
        DayOfWeek aDay = aRefDate.GetDayOfWeek();
        INT16 nDay;
        if ( aDay != SUNDAY )
            nDay = (INT16)aDay + 2;
        else
            nDay = 1;   // 1==Sonntag
        rPar.Get(0)->PutInteger( nDay );
    }
}

RTLFUNC(Year)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        INT16 nYear = implGetDateYear( rPar.Get(1)->GetDate() );
        rPar.Get(0)->PutInteger( nYear );
    }
}

RTLFUNC(Hour)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        double nArg = rPar.Get(1)->GetDate();
        if ( nArg < 0.0 )
            nArg *= -1.0;
        double nFrac = nArg - floor( nArg );
        nFrac *= 86400.0;
        INT32 nSeconds = (INT32)nFrac;
        INT16 nHour = (INT16)(nSeconds / 3600);
        rPar.Get(0)->PutInteger( nHour );
    }
}


RTLFUNC(Minute)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        double nArg = rPar.Get(1)->GetDate();
        if ( nArg < 0.0 )
            nArg *= -1.0;
        double nFrac = nArg - floor( nArg );
        nFrac *= 86400.0;
        INT32 nSeconds = (INT32)nFrac;
        INT16 nTemp = (INT16)(nSeconds % 3600);
        INT16 nMin = nTemp / 60;
        rPar.Get(0)->PutInteger( nMin );
    }
}

RTLFUNC(Month)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        INT16 nMonth = implGetDateMonth( rPar.Get(1)->GetDate() );
        rPar.Get(0)->PutInteger( nMonth );
    }
}

RTLFUNC(Second)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        double nArg = rPar.Get(1)->GetDate();
        if ( nArg < 0.0 )
            nArg *= -1.0;
        double nFrac = nArg - floor( nArg );
        nFrac *= 86400.0;
        INT32 nSeconds = (INT32)nFrac;
        INT16 nTemp = (INT16)(nSeconds / 3600);
        nSeconds -= nTemp * 3600;
        nTemp = (INT16)(nSeconds / 60);
        nSeconds -= nTemp * 60;
        rPar.Get(0)->PutInteger( (INT16)nSeconds );
    }
}

// Date Now(void)

RTLFUNC(Now)
{
    Date aDate;
    Time aTime;
    double aSerial = (double)GetDayDiff( aDate );
    long nSeconds = aTime.GetHour();
    nSeconds *= 3600;
    nSeconds += aTime.GetMin() * 60;
    nSeconds += aTime.GetSec();
    double nDays = ((double)nSeconds) / (double)(24.0*3600.0);
    aSerial += nDays;
    rPar.Get(0)->PutDate( aSerial );
}

// Date Time(void)

RTLFUNC(Time)
{
    if ( !bWrite )
    {
        Time aTime;
        SbxVariable* pMeth = rPar.Get( 0 );
        String aRes;
        if( pMeth->IsFixed() )
        {
            // Time$: hh:mm:ss
            char buf[ 20 ];
            snprintf( buf, sizeof(buf), "%02d:%02d:%02d",
                aTime.GetHour(), aTime.GetMin(), aTime.GetSec() );
            aRes = String::CreateFromAscii( buf );
        }
        else
        {
            // Time: system dependent
            long nSeconds=aTime.GetHour();
            nSeconds *= 3600;
            nSeconds += aTime.GetMin() * 60;
            nSeconds += aTime.GetSec();
            double nDays = (double)nSeconds * ( 1.0 / (24.0*3600.0) );
            Color* pCol;

            // #39629 pINST pruefen, kann aus URL-Zeile gerufen werden
            SvNumberFormatter* pFormatter = NULL;
            ULONG nIndex;
            if( pINST )
            {
                pFormatter = pINST->GetNumberFormatter();
                nIndex = pINST->GetStdTimeIdx();
            }
            else
            {
                ULONG n;    // Dummy
                SbiInstance::PrepareNumberFormatter( pFormatter, n, nIndex, n );
            }

            pFormatter->GetOutputString( nDays, nIndex, aRes, &pCol );

            // #39629 pFormatter kann selbst angefordert sein
            if( !pINST )
                delete pFormatter;
        }
        pMeth->PutString( aRes );
    }
    else
    {
        StarBASIC::Error( SbERR_NOT_IMPLEMENTED );
    }
}

RTLFUNC(Timer)
{
    Time aTime;
    long nSeconds = aTime.GetHour();
    nSeconds *= 3600;
    nSeconds += aTime.GetMin() * 60;
    nSeconds += aTime.GetSec();
    rPar.Get(0)->PutDate( (double)nSeconds );
}


RTLFUNC(Date)
{
    if ( !bWrite )
    {
        Date aToday;
        double nDays = (double)GetDayDiff( aToday );
        SbxVariable* pMeth = rPar.Get( 0 );
        if( pMeth->IsString() )
        {
            String aRes;
            Color* pCol;

            // #39629 pINST pruefen, kann aus URL-Zeile gerufen werden
            SvNumberFormatter* pFormatter = NULL;
            ULONG nIndex;
            if( pINST )
            {
                pFormatter = pINST->GetNumberFormatter();
                nIndex = pINST->GetStdDateIdx();
            }
            else
            {
                ULONG n;    // Dummy
                SbiInstance::PrepareNumberFormatter( pFormatter, nIndex, n, n );
            }

            pFormatter->GetOutputString( nDays, nIndex, aRes, &pCol );
            pMeth->PutString( aRes );

            // #39629 pFormatter kann selbst angefordert sein
            if( !pINST )
                delete pFormatter;
        }
        else
            pMeth->PutDate( nDays );
    }
    else
    {
        StarBASIC::Error( SbERR_NOT_IMPLEMENTED );
    }
}

RTLFUNC(IsArray)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
        rPar.Get(0)->PutBool((rPar.Get(1)->GetType() & SbxARRAY) ? TRUE : FALSE );
}

RTLFUNC(IsObject)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbxVariable* pVar = rPar.Get(1);
        SbxBase* pObj = (SbxBase*)pVar->GetObject();

        // #100385: GetObject can result in an error, so reset it
        SbxBase::ResetError();

        SbUnoClass* pUnoClass;
        BOOL bObject;
        if( pObj &&  NULL != ( pUnoClass=PTR_CAST(SbUnoClass,pObj) ) )
        {
            bObject = pUnoClass->getUnoClass().is();
        }
        else
        {
            bObject = pVar->IsObject();
        }
        rPar.Get( 0 )->PutBool( bObject );
    }
}

RTLFUNC(IsDate)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        // #46134 Nur String wird konvertiert, andere Typen ergeben FALSE
        SbxVariableRef xArg = rPar.Get( 1 );
        SbxDataType eType = xArg->GetType();
        BOOL bDate = FALSE;

        if( eType == SbxDATE )
        {
            bDate = TRUE;
        }
        else if( eType == SbxSTRING )
        {
            // Error loeschen
            SbxError nPrevError = SbxBase::GetError();
            SbxBase::ResetError();

            // Konvertierung des Parameters nach SbxDATE erzwingen
            xArg->SbxValue::GetDate();

            // Bei Fehler ist es kein Date
            bDate = !SbxBase::IsError();

            // Error-Situation wiederherstellen
            SbxBase::ResetError();
            SbxBase::SetError( nPrevError );
        }
        rPar.Get( 0 )->PutBool( bDate );
    }
}

RTLFUNC(IsEmpty)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
        rPar.Get( 0 )->PutBool( rPar.Get(1)->IsEmpty() );
}

RTLFUNC(IsError)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
        rPar.Get( 0 )->PutBool( rPar.Get(1)->IsErr() );
}

RTLFUNC(IsNull)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        // #51475 Wegen Uno-Objekten auch true liefern,
        // wenn der pObj-Wert NULL ist
        SbxVariableRef pArg = rPar.Get( 1 );
        BOOL bNull = rPar.Get(1)->IsNull();
        if( !bNull && pArg->GetType() == SbxOBJECT )
        {
            SbxBase* pObj = pArg->GetObject();
            if( !pObj )
                bNull = TRUE;
        }
        rPar.Get( 0 )->PutBool( bNull );
    }
}

RTLFUNC(IsNumeric)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
        rPar.Get( 0 )->PutBool( rPar.Get( 1 )->IsNumericRTL() );
}

// Das machen wir auf die billige Tour

RTLFUNC(IsMissing)
{
    if ( rPar.Count() < 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
        // #57915 Missing wird durch Error angezeigt
        rPar.Get( 0 )->PutBool( rPar.Get(1)->IsErr() );
}

// Dir( [Maske] [,Attrs] )
// ToDo: Library-globaler Datenbereich fuer Dir-Objekt und Flags


String getDirectoryPath( String aPathStr )
{
    String aRetStr;

    DirectoryItem aItem;
    FileBase::RC nRet = DirectoryItem::get( aPathStr, aItem );
    if( nRet == FileBase::E_None )
    {
        FileStatus aFileStatus( FileStatusMask_Type );
        nRet = aItem.getFileStatus( aFileStatus );
        if( nRet == FileBase::E_None )
        {
            FileStatus::Type aType = aFileStatus.getFileType();
            if( isFolder( aType ) )
            {
                aRetStr = aPathStr;
            }
            else if( aType == FileStatus::Link )
            {
                FileStatus aFileStatus( FileStatusMask_LinkTargetURL );
                nRet = aItem.getFileStatus( aFileStatus );
                if( nRet == FileBase::E_None )
                    aRetStr = getDirectoryPath( aFileStatus.getLinkTargetURL() );
            }
        }
    }
    return aRetStr;
}

// #80200 HACK to provide minimum wildcard functionality (finally solution in UCB)
// Function looks for wildcards, removes them and always returns the pure path
String implSetupWildcard( const String& rFileParam, SbiRTLData* pRTLData )
{
    static String aAsterisk = String::CreateFromAscii( "*" );
    static sal_Char cDelim1 = (sal_Char)'/';
    static sal_Char cDelim2 = (sal_Char)'\\';

    pRTLData->bDoCheck = sal_False;
    pRTLData->bNeedsDot = sal_False;
    pRTLData->sExtension = String();
    pRTLData->sPreWildcard = String();
    pRTLData->sFullNameToBeChecked = String();

    sal_Char cWild = '*';
    String aFileParam = rFileParam;
    xub_StrLen nLastWild = aFileParam.SearchBackward( cWild );
    sal_Bool bHasWildcards = ( nLastWild != STRING_NOTFOUND );


    xub_StrLen nLastDelim = aFileParam.SearchBackward( cDelim1 );
    if( nLastDelim == STRING_NOTFOUND )
        nLastDelim = aFileParam.SearchBackward( cDelim2 );

    if( bHasWildcards )
    {
        // Wildcards in path?
        if( nLastDelim != STRING_NOTFOUND && nLastDelim > nLastWild )
            return aFileParam;
    }
    else
    {
        String aPathStr = getFullPath( aFileParam );
        if( nLastDelim != aFileParam.Len() - 1 )
            pRTLData->sFullNameToBeChecked = aPathStr;
        return aPathStr;
    }

    String aPureFileName;
    if( nLastDelim == STRING_NOTFOUND )
    {
        aPureFileName = aFileParam;
        aFileParam = String();
    }
    else
    {
        aPureFileName = aFileParam.Copy( nLastDelim + 1 );
        aFileParam = aFileParam.Copy( 0, nLastDelim );
    }

    // Try again to get a valid URL/UNC-path with only the path
    String aPathStr = getFullPath( aFileParam );
    xub_StrLen nPureLen = aPureFileName.Len();

    // Is there a pure file name left? Otherwise the path is
    // invalid anyway because it was not accepted by OSL before
    if( nPureLen && aPureFileName != aAsterisk )
    {
        // We make it very easy and search only for "anything*.xxx" patterns
        sal_Char cDot = '.';

        xub_StrLen nLastWild = aPureFileName.SearchBackward( cWild );
        xub_StrLen nLastDot = aPureFileName.SearchBackward( cDot );

        // Handle ".*" and "x*" extensions
        sal_Bool bAnyExtension = sal_False;
        pRTLData->bNeedsDot = (nLastDot != STRING_NOTFOUND);
        if( !pRTLData->bNeedsDot )
        {
            bAnyExtension = sal_True;
        }
        else if( nLastWild == nPureLen-1 )
        {
            if( nLastDot == nPureLen-2 )
            {
                bAnyExtension = sal_True;
                nLastWild = aPureFileName.SearchBackward( cWild, nLastDot );
            }
            else if( nLastDot == STRING_NOTFOUND )
            {
                bAnyExtension = sal_True;
            }
        }

        if( nLastWild == nLastDot-1 ||
            ( nLastDot == STRING_NOTFOUND && bAnyExtension ) )
        {
            pRTLData->bDoCheck = sal_True;
            if( !bAnyExtension )
                pRTLData->sExtension = aPureFileName.Copy( nLastDot + 1 );
            pRTLData->sPreWildcard = aPureFileName.Copy( 0, nLastWild );
        }
    }
    return aPathStr;
}

sal_Bool implCheckWildcard( const String& rName, SbiRTLData* pRTLData )
{
    sal_Bool bMatch = sal_True;

    // #80200 HACK to provide minimum wildcard functionality (*.xxx)
    if( pRTLData->bDoCheck )
    {
        bMatch = sal_False;

        sal_Char cDot = '.';    // char -> sal_Char ok, because ASCII
        xub_StrLen nLastDot = rName.SearchBackward( cDot );

        String sExtension, sMainName;
        if( nLastDot == STRING_NOTFOUND )
        {
            sMainName = rName;
        }
        else
        {
            sExtension = rName.Copy( nLastDot + 1 );
            sMainName = rName.Copy( 0, nLastDot );
        }

        if( nLastDot != STRING_NOTFOUND || !pRTLData->bNeedsDot )
        {
            bool bExtensionMatch = true;
            if( pRTLData->sExtension.Len() )
            {
#ifdef UNX
                bExtensionMatch = sExtension.Equals( pRTLData->sExtension );
#else
                bExtensionMatch = sExtension.EqualsIgnoreCaseAscii( pRTLData->sExtension );
#endif
            }

            if( bExtensionMatch )
            {
                sal_Int32 nPreWildcardLen = pRTLData->sPreWildcard.Len();
                if( nPreWildcardLen )
                {
                    String sTmp = sMainName.Copy( 0, (sal_Int32)nPreWildcardLen );
#ifdef UNX
                    bMatch = sTmp.Equals( pRTLData->sPreWildcard );
#else
                    bMatch = sTmp.EqualsIgnoreCaseAscii( pRTLData->sPreWildcard );
#endif
                }
                else
                {
                    bMatch = sal_True;
                }
            }
        }
    }
    return bMatch;
}

// End #80200 HACK


bool isRootDir( String aDirURLStr )
{
    INetURLObject aDirURLObj( aDirURLStr );
    BOOL bRoot = FALSE;

    // Check if it's a root directory
    sal_Int32 nCount = aDirURLObj.getSegmentCount();

    // No segment means Unix root directory "file:///"
    if( nCount == 0 )
    {
        bRoot = TRUE;
    }
    // Exactly one segment needs further checking, because it
    // can be Unix "file:///foo/" -> no root
    // or Windows  "file:///c:/"  -> root
    else if( nCount == 1 )
    {
        OUString aSeg1 = aDirURLObj.getName( 0, TRUE,
            INetURLObject::DECODE_WITH_CHARSET );
        if( aSeg1.getStr()[1] == (sal_Unicode)':' )
        {
            bRoot = TRUE;
        }
    }
    // More than one segments can never be root
    // so bRoot remains FALSE

    return bRoot;
}

RTLFUNC(Dir)
{
    String aPath;

    USHORT nParCount = rPar.Count();
    if( nParCount > 3 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbiRTLData* pRTLData = pINST->GetRTLData();

        // #34645: Kann auch von der URL-Zeile ueber 'macro: Dir' aufgerufen werden
        // dann existiert kein pRTLData und die Methode muss verlassen werden
        if( !pRTLData )
            return;

        // <-- UCB
        if( hasUno() )
        {
            Reference< XSimpleFileAccess3 > xSFI = getFileAccess();
            if( xSFI.is() )
            {
                if ( nParCount >= 2 )
                {
                    String aFileParam = rPar.Get(1)->GetString();

                    // #80200 HACK to provide minimum wildcard functionality
                    String aFileURLStr = implSetupWildcard( aFileParam, pRTLData );
                    if( pRTLData->sFullNameToBeChecked.Len() > 0 )
                    {
                        sal_Bool bExists = sal_False;
                        try { bExists = xSFI->exists( aFileURLStr ); }
                        catch( Exception & ) {}

                        String aNameOnlyStr;
                        if( bExists )
                        {
                            INetURLObject aFileURL( aFileURLStr );
                            aNameOnlyStr = aFileURL.getName( INetURLObject::LAST_SEGMENT,
                                true, INetURLObject::DECODE_WITH_CHARSET );
                        }
                        rPar.Get(0)->PutString( aNameOnlyStr );
                        return;
                    }

                    try
                    {
                        String aDirURLStr;
                        sal_Bool bFolder = xSFI->isFolder( aFileURLStr );

                        if( bFolder )
                        {
                            aDirURLStr = aFileURLStr;
                        }
                        else
                        {
                            String aEmptyStr;
                            rPar.Get(0)->PutString( aEmptyStr );
                        }

                        USHORT nFlags = 0;
                        if ( nParCount > 2 )
                            pRTLData->nDirFlags = nFlags = rPar.Get(2)->GetInteger();
                        else
                            pRTLData->nDirFlags = 0;

                        // Read directory
                        sal_Bool bIncludeFolders = ((nFlags & Sb_ATTR_DIRECTORY) != 0);
                        pRTLData->aDirSeq = xSFI->getFolderContents( aDirURLStr, bIncludeFolders );
                        pRTLData->nCurDirPos = 0;

                        // #78651 Add "." and ".." directories for VB compatibility
                        if( bIncludeFolders )
                        {
                            BOOL bRoot = isRootDir( aDirURLStr );

                            // If it's no root directory we flag the need for
                            // the "." and ".." directories by the value -2
                            // for the actual position. Later for -2 will be
                            // returned "." and for -1 ".."
                            if( !bRoot )
                            {
                                pRTLData->nCurDirPos = -2;
                            }
                        }
                    }
                    catch( Exception & )
                    {
                        //StarBASIC::Error( ERRCODE_IO_GENERAL );
                    }
                }


                if( pRTLData->aDirSeq.getLength() > 0 )
                {
                    sal_Bool bOnlyFolders = ((pRTLData->nDirFlags & Sb_ATTR_DIRECTORY) != 0);
                    for( ;; )
                    {
                        if( pRTLData->nCurDirPos < 0 )
                        {
                            if( pRTLData->nCurDirPos == -2 )
                            {
                                aPath = OUString::createFromAscii( "." );
                            }
                            else if( pRTLData->nCurDirPos == -1 )
                            {
                                aPath = OUString::createFromAscii( ".." );
                            }
                            pRTLData->nCurDirPos++;
                        }
                        else if( pRTLData->nCurDirPos >= pRTLData->aDirSeq.getLength() )
                        {
                            pRTLData->aDirSeq.realloc( 0 );
                            aPath.Erase();
                            break;
                        }
                        else
                        {
                            OUString aFile = pRTLData->aDirSeq.getConstArray()[pRTLData->nCurDirPos++];

                            // Only directories?
                            if( bOnlyFolders )
                            {
                                sal_Bool bFolder = xSFI->isFolder( aFile );
                                if( !bFolder )
                                    continue;
                            }

                            INetURLObject aURL( aFile );
                            aPath = aURL.getName( INetURLObject::LAST_SEGMENT, TRUE,
                                INetURLObject::DECODE_WITH_CHARSET );
                        }

                        // #80200 HACK to provide minimum wildcard functionality (*.xxx)
                        sal_Bool bMatch = implCheckWildcard( aPath, pRTLData );
                        if( !bMatch )
                            continue;
                        // End #80200 HACK

                        break;
                    }
                }
                rPar.Get(0)->PutString( aPath );
            }
        }
        else
        // --> UCB
        {
#ifdef _OLD_FILE_IMPL
            if ( nParCount >= 2 )
            {
                delete pRTLData->pDir;
                pRTLData->pDir = 0; // wg. Sonderbehandlung Sb_ATTR_VOLUME
                DirEntry aEntry( rPar.Get(1)->GetString() );
                FileStat aStat( aEntry );
                if(!aStat.GetError() && (aStat.GetKind() & FSYS_KIND_FILE))
                {
                    // ah ja, ist nur ein dateiname
                    // Pfad abschneiden (wg. VB4)
                    rPar.Get(0)->PutString( aEntry.GetName() );
                    return;
                }
                USHORT nFlags = 0;
                if ( nParCount > 2 )
                    pRTLData->nDirFlags = nFlags = rPar.Get(2)->GetInteger();
                else
                    pRTLData->nDirFlags = 0;
                // Nur diese Bitmaske ist unter Windows erlaubt
    #ifdef WIN
                if( nFlags & ~0x1E )
                    StarBASIC::Error( SbERR_BAD_ARGUMENT ), pRTLData->nDirFlags = 0;
    #endif
                // Sb_ATTR_VOLUME wird getrennt gehandelt
                if( pRTLData->nDirFlags & Sb_ATTR_VOLUME )
                    aPath = aEntry.GetVolume();
                else
                {
                    // Die richtige Auswahl treffen
                    USHORT nMode = FSYS_KIND_FILE;
                    if( nFlags & Sb_ATTR_DIRECTORY )
                        nMode |= FSYS_KIND_DIR;
                    if( nFlags == Sb_ATTR_DIRECTORY )
                        nMode = FSYS_KIND_DIR;
                    pRTLData->pDir = new Dir( aEntry, (DirEntryKind) nMode );
                    pRTLData->nCurDirPos = 0;
                }
            }

            if( pRTLData->pDir )
            {
                for( ;; )
                {
                    if( pRTLData->nCurDirPos >= pRTLData->pDir->Count() )
                    {
                        delete pRTLData->pDir;
                        pRTLData->pDir = 0;
                        aPath.Erase();
                        break;
                    }
                    DirEntry aNextEntry=(*(pRTLData->pDir))[pRTLData->nCurDirPos++];
                    aPath = aNextEntry.GetName(); //Full();
    #ifdef WIN
                    aNextEntry.ToAbs();
                    String sFull(aNextEntry.GetFull());
                    unsigned nFlags;

                    if (_dos_getfileattr( sFull.GetStr(), &nFlags ))
                        StarBASIC::Error( SbERR_FILE_NOT_FOUND );
                    else
                    {
                        INT16 nCurFlags = pRTLData->nDirFlags;
                        if( (nCurFlags == Sb_ATTR_NORMAL)
                          && !(nFlags & ( _A_HIDDEN | _A_SYSTEM | _A_VOLID | _A_SUBDIR ) ) )
                            break;
                        else if( (nCurFlags & Sb_ATTR_HIDDEN) && (nFlags & _A_HIDDEN) )
                            break;
                        else if( (nCurFlags & Sb_ATTR_SYSTEM) && (nFlags & _A_SYSTEM) )
                            break;
                        else if( (nCurFlags & Sb_ATTR_VOLUME) && (nFlags & _A_VOLID) )
                            break;
                        else if( (nCurFlags & Sb_ATTR_DIRECTORY) && (nFlags & _A_SUBDIR) )
                            break;
                    }
    #else
                    break;
    #endif
                }
            }
            rPar.Get(0)->PutString( aPath );
#else
            // TODO: OSL
            if ( nParCount >= 2 )
            {
                String aFileParam = rPar.Get(1)->GetString();

                // #80200 HACK to provide minimum wildcard functionality
                String aDirURL = implSetupWildcard( aFileParam, pRTLData );

                USHORT nFlags = 0;
                if ( nParCount > 2 )
                    pRTLData->nDirFlags = nFlags = rPar.Get(2)->GetInteger();
                else
                    pRTLData->nDirFlags = 0;

                // Read directory
                sal_Bool bIncludeFolders = ((nFlags & Sb_ATTR_DIRECTORY) != 0);
                pRTLData->pDir = new Directory( aDirURL );
                FileBase::RC nRet = pRTLData->pDir->open();
                if( nRet != FileBase::E_None )
                {
                    delete pRTLData->pDir;
                    pRTLData->pDir = NULL;
                    rPar.Get(0)->PutString( String() );
                    return;
                }

                // #86950 Add "." and ".." directories for VB compatibility
                pRTLData->nCurDirPos = 0;
                if( bIncludeFolders )
                {
                    BOOL bRoot = isRootDir( aDirURL );

                    // If it's no root directory we flag the need for
                    // the "." and ".." directories by the value -2
                    // for the actual position. Later for -2 will be
                    // returned "." and for -1 ".."
                    if( !bRoot )
                    {
                        pRTLData->nCurDirPos = -2;
                    }
                }

            }

            if( pRTLData->pDir )
            {
                sal_Bool bOnlyFolders = ((pRTLData->nDirFlags & Sb_ATTR_DIRECTORY) != 0);
                for( ;; )
                {
                    if( pRTLData->nCurDirPos < 0 )
                    {
                        if( pRTLData->nCurDirPos == -2 )
                        {
                            aPath = OUString::createFromAscii( "." );
                        }
                        else if( pRTLData->nCurDirPos == -1 )
                        {
                            aPath = OUString::createFromAscii( ".." );
                        }
                        pRTLData->nCurDirPos++;
                    }
                    else
                    {
                        DirectoryItem aItem;
                        FileBase::RC nRet = pRTLData->pDir->getNextItem( aItem );
                        if( nRet != FileBase::E_None )
                        {
                            delete pRTLData->pDir;
                            pRTLData->pDir = NULL;
                            aPath.Erase();
                            break;
                        }

                        // Handle flags
                        FileStatus aFileStatus( FileStatusMask_Type | FileStatusMask_FileName );
                        nRet = aItem.getFileStatus( aFileStatus );

                        // Only directories?
                        if( bOnlyFolders )
                        {
                            FileStatus::Type aType = aFileStatus.getFileType();
                            sal_Bool bFolder = isFolder( aType );
                            if( !bFolder )
                                continue;
                        }

                        aPath = aFileStatus.getFileName();
                    }

                    // #80200 HACK to provide minimum wildcard functionality (*.xxx)
                    sal_Bool bMatch = implCheckWildcard( aPath, pRTLData );
                    if( !bMatch )
                        continue;
                    // End #80200 HACK

                    break;
                }
            }
            rPar.Get(0)->PutString( aPath );
#endif
        }
    }
}


RTLFUNC(GetAttr)
{
    if ( rPar.Count() == 2 )
    {
        INT16 nFlags = 0;

        // <-- UCB
        if( hasUno() )
        {
            Reference< XSimpleFileAccess3 > xSFI = getFileAccess();
            if( xSFI.is() )
            {
                try
                {
                    String aPath = getFullPath( rPar.Get(1)->GetString() );
                    sal_Bool bExists = sal_False;
                    try { bExists = xSFI->exists( aPath ); }
                    catch( Exception & ) {}
                    if( !bExists )
                    {
                        StarBASIC::Error( SbERR_FILE_NOT_FOUND );
                        return;
                    }

                    sal_Bool bReadOnly = xSFI->isReadOnly( aPath );
                    sal_Bool bHidden = xSFI->isHidden( aPath );
                    sal_Bool bDirectory = xSFI->isFolder( aPath );
                    if( bReadOnly )
                        nFlags |= 0x0001; // ATTR_READONLY
                    if( bHidden )
                        nFlags |= 0x0002; // ATTR_HIDDEN
                    if( bDirectory )
                        nFlags |= 0x0010; // ATTR_DIRECTORY
                }
                catch( Exception & )
                {
                    StarBASIC::Error( ERRCODE_IO_GENERAL );
                }
            }
        }
        else
        // --> UCB
        {
#ifdef _OLD_FILE_IMPL
            DirEntry aEntry( rPar.Get(1)->GetString() );
            aEntry.ToAbs();
            BOOL bUseFileStat = FALSE;

            // #57064 Bei virtuellen URLs den Real-Path extrahieren
            String aFile = aEntry.GetFull();
            ByteString aByteStrFullPath( aEntry.GetFull(), gsl_getSystemTextEncoding() );
    #if defined( WIN )
            int nErr = _dos_getfileattr( aByteStrFullPath.GetBuffer(),(unsigned *) &nFlags );
            if ( nErr )
                StarBASIC::Error( SbERR_FILE_NOT_FOUND );
    #elif defined( WNT )
            DWORD nRealFlags = GetFileAttributes (aByteStrFullPath.GetBuffer());
            if (nRealFlags != 0xffffffff)
            {
                if (nRealFlags == FILE_ATTRIBUTE_NORMAL)
                    nRealFlags = 0;
                nFlags = (INT16) (nRealFlags);
            }
            else
                StarBASIC::Error( SbERR_FILE_NOT_FOUND );
    #elif defined( OS2 )
            FILESTATUS3 aFileStatus;
            APIRET rc = DosQueryPathInfo(aByteStrFullPath.GetBuffer(),1,
                                         &aFileStatus,sizeof(FILESTATUS3));
            if (!rc)
                nFlags = (INT16) aFileStatus.attrFile;
            else
                StarBASIC::Error( SbERR_FILE_NOT_FOUND );
    #else
            bUseFileStat = TRUE;
    #endif
            if( bUseFileStat )
            {
                if( FileStat::GetReadOnlyFlag( aEntry ) )
                    nFlags |= 0x0001; // ATTR_READONLY
                FileStat aStat( aEntry );
                DirEntryKind eKind = aStat.GetKind();
                if( eKind & FSYS_KIND_DIR )
                    nFlags |= 0x0010; // ATTR_DIRECTORY
                if( aEntry.GetFlag() & FSYS_FLAG_VOLUME )
                    nFlags |= 0x0008; // ATTR_VOLUME
            }
#else
            DirectoryItem aItem;
            FileBase::RC nRet = DirectoryItem::get( getFullPathUNC( rPar.Get(1)->GetString() ), aItem );
            FileStatus aFileStatus( FileStatusMask_Attributes | FileStatusMask_Type );
            nRet = aItem.getFileStatus( aFileStatus );
            sal_uInt64 nAttributes = aFileStatus.getAttributes();
            sal_Bool bReadOnly = (nAttributes & Attribute_ReadOnly) != 0;

            FileStatus::Type aType = aFileStatus.getFileType();
            sal_Bool bDirectory = isFolder( aType );
            if( bReadOnly )
                nFlags |= 0x0001; // ATTR_READONLY
            if( bDirectory )
                nFlags |= 0x0010; // ATTR_DIRECTORY

#endif
        }
        rPar.Get(0)->PutInteger( nFlags );
    }
    else
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
}


RTLFUNC(FileDateTime)
{
    if ( rPar.Count() != 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        // <-- UCB
        String aPath = rPar.Get(1)->GetString();
        Time aTime;
        Date aDate;
        if( hasUno() )
        {
            Reference< XSimpleFileAccess3 > xSFI = getFileAccess();
            if( xSFI.is() )
            {
                try
                {
                    com::sun::star::util::DateTime aUnoDT = xSFI->getDateTimeModified( aPath );
                    aTime = Time( aUnoDT.Hours, aUnoDT.Minutes, aUnoDT.Seconds, aUnoDT.HundredthSeconds );
                    aDate = Date( aUnoDT.Day, aUnoDT.Month, aUnoDT.Year );
                }
                catch( Exception & )
                {
                    StarBASIC::Error( ERRCODE_IO_GENERAL );
                }
            }
        }
        else
        // --> UCB
        {
#ifdef _OLD_FILE_IMPL
            DirEntry aEntry( aPath );
            FileStat aStat( aEntry );
            aTime = Time( aStat.TimeModified() );
            aDate = Date( aStat.DateModified() );
#else
            DirectoryItem aItem;
            FileBase::RC nRet = DirectoryItem::get( getFullPathUNC( aPath ), aItem );
            FileStatus aFileStatus( FileStatusMask_ModifyTime );
            nRet = aItem.getFileStatus( aFileStatus );
            TimeValue aTimeVal = aFileStatus.getModifyTime();
            oslDateTime aDT;
            sal_Bool bRet = osl_getDateTimeFromTimeValue( &aTimeVal, &aDT );

            aTime = Time( aDT.Hours, aDT.Minutes, aDT.Seconds, 10000000*aDT.NanoSeconds );
            aDate = Date( aDT.Day, aDT.Month, aDT.Year );
#endif
        }

        double fSerial = (double)GetDayDiff( aDate );
        long nSeconds = aTime.GetHour();
        nSeconds *= 3600;
        nSeconds += aTime.GetMin() * 60;
        nSeconds += aTime.GetSec();
        double nDays = ((double)nSeconds) / (double)(24.0*3600.0);
        fSerial += nDays;

        Color* pCol;

        // #39629 pINST pruefen, kann aus URL-Zeile gerufen werden
        SvNumberFormatter* pFormatter = NULL;
        ULONG nIndex;
        if( pINST )
        {
            pFormatter = pINST->GetNumberFormatter();
            nIndex = pINST->GetStdDateTimeIdx();
        }
        else
        {
            ULONG n;    // Dummy
            SbiInstance::PrepareNumberFormatter( pFormatter, n, n, nIndex );
        }

        String aRes;
        pFormatter->GetOutputString( fSerial, nIndex, aRes, &pCol );
        rPar.Get(0)->PutString( aRes );

        // #39629 pFormatter kann selbst angefordert sein
        if( !pINST )
            delete pFormatter;
    }
}


RTLFUNC(EOF)
{
    // AB 08/16/2000: No changes for UCB
    if ( rPar.Count() != 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        INT16 nChannel = rPar.Get(1)->GetInteger();
        // nChannel--;  // macht MD beim Oeffnen auch nicht
        SbiIoSystem* pIO = pINST->GetIoSystem();
        SbiStream* pSbStrm = pIO->GetStream( nChannel );
        if ( !pSbStrm )
        {
            StarBASIC::Error( SbERR_BAD_CHANNEL );
            return;
        }
        BOOL bIsEof;
        SvStream* pSvStrm = pSbStrm->GetStrm();
        if ( pSbStrm->IsText() )
        {
            char cBla;
            (*pSvStrm) >> cBla; // koennen wir noch ein Zeichen lesen
            bIsEof = pSvStrm->IsEof();
            if ( !bIsEof )
                pSvStrm->SeekRel( -1 );
        }
        else
            bIsEof = pSvStrm->IsEof();  // fuer binaerdateien!
        rPar.Get(0)->PutBool( bIsEof );
    }
}

RTLFUNC(FileAttr)
{
    // AB 08/16/2000: No changes for UCB

    // #57064 Obwohl diese Funktion nicht mit DirEntry arbeitet, ist sie von
    // der Anpassung an virtuelle URLs nich betroffen, da sie nur auf bereits
    // geoeffneten Dateien arbeitet und der Name hier keine Rolle spielt.

    if ( rPar.Count() != 3 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        INT16 nChannel = rPar.Get(1)->GetInteger();
//      nChannel--;
        SbiIoSystem* pIO = pINST->GetIoSystem();
        SbiStream* pSbStrm = pIO->GetStream( nChannel );
        if ( !pSbStrm )
        {
            StarBASIC::Error( SbERR_BAD_CHANNEL );
            return;
        }
        INT16 nRet;
        if ( rPar.Get(2)->GetInteger() == 1 )
            nRet = (INT16)(pSbStrm->GetMode());
        else
            nRet = 0; // System file handle not supported

        rPar.Get(0)->PutInteger( nRet );
    }
}
RTLFUNC(Loc)
{
    // AB 08/16/2000: No changes for UCB
    if ( rPar.Count() != 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        INT16 nChannel = rPar.Get(1)->GetInteger();
        SbiIoSystem* pIO = pINST->GetIoSystem();
        SbiStream* pSbStrm = pIO->GetStream( nChannel );
        if ( !pSbStrm )
        {
            StarBASIC::Error( SbERR_BAD_CHANNEL );
            return;
        }
        SvStream* pSvStrm = pSbStrm->GetStrm();
        ULONG nPos;
        if( pSbStrm->IsRandom())
        {
            short nBlockLen = pSbStrm->GetBlockLen();
            nPos = nBlockLen ? (pSvStrm->Tell() / nBlockLen) : 0;
            nPos++; // Blockpositionen beginnen bei 1
        }
        else if ( pSbStrm->IsText() )
            nPos = pSbStrm->GetLine();
        else if( pSbStrm->IsBinary() )
            nPos = pSvStrm->Tell();
        else if ( pSbStrm->IsSeq() )
            nPos = ( pSvStrm->Tell()+1 ) / 128;
        else
            nPos = pSvStrm->Tell();
        rPar.Get(0)->PutLong( (INT32)nPos );
    }
}

RTLFUNC(Lof)
{
    // AB 08/16/2000: No changes for UCB
    if ( rPar.Count() != 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        INT16 nChannel = rPar.Get(1)->GetInteger();
        SbiIoSystem* pIO = pINST->GetIoSystem();
        SbiStream* pSbStrm = pIO->GetStream( nChannel );
        if ( !pSbStrm )
        {
            StarBASIC::Error( SbERR_BAD_CHANNEL );
            return;
        }
        SvStream* pSvStrm = pSbStrm->GetStrm();
        ULONG nOldPos = pSvStrm->Tell();
        ULONG nLen = pSvStrm->Seek( STREAM_SEEK_TO_END );
        pSvStrm->Seek( nOldPos );
        rPar.Get(0)->PutLong( (INT32)nLen );
    }
}


RTLFUNC(Seek)
{
    // AB 08/16/2000: No changes for UCB
    int nArgs = (int)rPar.Count();
    if ( nArgs < 2 || nArgs > 3 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }
    INT16 nChannel = rPar.Get(1)->GetInteger();
//  nChannel--;
    SbiIoSystem* pIO = pINST->GetIoSystem();
    SbiStream* pSbStrm = pIO->GetStream( nChannel );
    if ( !pSbStrm )
    {
        StarBASIC::Error( SbERR_BAD_CHANNEL );
        return;
    }
    SvStream* pStrm = pSbStrm->GetStrm();

    if ( nArgs == 2 )   // Seek-Function
    {
        ULONG nPos = pStrm->Tell();
        if( pSbStrm->IsRandom() )
            nPos = nPos / pSbStrm->GetBlockLen();
        nPos++; // Basic zaehlt ab 1
        rPar.Get(0)->PutLong( (INT32)nPos );
    }
    else                // Seek-Statement
    {
        INT32 nPos = rPar.Get(2)->GetLong();
        if ( nPos < 1 )
        {
            StarBASIC::Error( SbERR_BAD_ARGUMENT );
            return;
        }
        nPos--; // Basic zaehlt ab 1, SvStreams zaehlen ab 0
        pSbStrm->SetExpandOnWriteTo( 0 );
        if ( pSbStrm->IsRandom() )
            nPos *= pSbStrm->GetBlockLen();
        pStrm->Seek( (ULONG)nPos );
        pSbStrm->SetExpandOnWriteTo( nPos );
    }
}

RTLFUNC(Format)
{
    USHORT nArgCount = (USHORT)rPar.Count();
    if ( nArgCount < 2 || nArgCount > 3 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        String aResult;
        if( nArgCount == 2 )
            rPar.Get(1)->Format( aResult );
        else
        {
            String aFmt( rPar.Get(2)->GetString() );
            rPar.Get(1)->Format( aResult, &aFmt );
        }
        rPar.Get(0)->PutString( aResult );
    }
}

RTLFUNC(Randomize)
{
    if ( rPar.Count() > 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    INT16 nSeed;
    if( rPar.Count() == 2 )
        nSeed = (INT16)rPar.Get(1)->GetInteger();
    else
        nSeed = (INT16)rand();
    srand( nSeed );
}

RTLFUNC(Rnd)
{
    if ( rPar.Count() > 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        double nRand = (double)rand();
        nRand = ( nRand / (double)RAND_MAX );
        rPar.Get(0)->PutDouble( nRand );
    }
}


//
//  Syntax: Shell("Path",[ Window-Style,[ "Params", [ bSync = FALSE ]]])
//
//  WindowStyles (VBA-kompatibel):
//      2 == Minimized
//      3 == Maximized
//     10 == Full-Screen (Textmodus-Anwendungen OS/2, WIN95, WNT)
//
// !!!HACK der WindowStyle wird im Creator an Application::StartApp
//         uebergeben. Format: "xxxx2"
//


RTLFUNC(Shell)
{
    // No shell command for "virtual" portal users
    if( needSecurityRestrictions() )
    {
        StarBASIC::Error(SbERR_NOT_IMPLEMENTED);
        return;
    }

    ULONG nArgCount = rPar.Count();
    if ( nArgCount < 2 || nArgCount > 5 )
    {
        rPar.Get(0)->PutLong(0);
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    }
    else
    {
        USHORT nOptions = NAMESPACE_VOS(OProcess)::TOption_SearchPath|
                          NAMESPACE_VOS(OProcess)::TOption_Detached;
        String aCmdLine = rPar.Get(1)->GetString();
        // Zusaetzliche Parameter anhaengen, es muss eh alles geparsed werden
        if( nArgCount >= 4 )
        {
            aCmdLine.AppendAscii( " " );
            aCmdLine += rPar.Get(3)->GetString();
        }
        else if( !aCmdLine.Len() )
        {
            // Spezial-Behandlung (leere Liste) vermeiden
            aCmdLine.AppendAscii( " " );
        }
        USHORT nLen = aCmdLine.Len();

        // #55735 Wenn Parameter dabei sind, muessen die abgetrennt werden
        // #72471 Auch die einzelnen Parameter trennen
        std::list<String> aTokenList;
        String aToken;
        USHORT i = 0;
        char c;
        while( i < nLen )
        {
            // Spaces weg
            while( ( c = aCmdLine.GetBuffer()[ i ] ) == ' ' || c == '\t' )
                i++;

            if( c == '\"' || c == '\'' )
            {
                USHORT iFoundPos = aCmdLine.Search( c, i + 1 );

                // Wenn nichts gefunden wurde, Rest kopieren
                if( iFoundPos == STRING_NOTFOUND )
                {
                    aToken = aCmdLine.Copy( i, STRING_LEN );
                    i = nLen;
                }
                else
                {
                    aToken = aCmdLine.Copy( i + 1, (iFoundPos - i - 1) );
                    i = iFoundPos + 1;
                }
            }
            else
            {
                USHORT iFoundSpacePos = aCmdLine.Search( ' ', i );
                USHORT iFoundTabPos = aCmdLine.Search( '\t', i );
                USHORT iFoundPos = Min( iFoundSpacePos, iFoundTabPos );

                // Wenn nichts gefunden wurde, Rest kopieren
                if( iFoundPos == STRING_NOTFOUND )
                {
                    aToken = aCmdLine.Copy( i, STRING_LEN );
                    i = nLen;
                }
                else
                {
                    aToken = aCmdLine.Copy( i, (iFoundPos - i) );
                    i = iFoundPos;
                }
            }

            // In die Liste uebernehmen
            aTokenList.push_back( aToken );
        }
        // #55735 / #72471 Ende

        INT16 nWinStyle = 0;
        if( nArgCount >= 3 )
        {
            nWinStyle = rPar.Get(2)->GetInteger();
            switch( nWinStyle )
            {
                case 2:
                    nOptions |= NAMESPACE_VOS(OProcess)::TOption_Minimized;
                    break;
                case 3:
                    nOptions |= NAMESPACE_VOS(OProcess)::TOption_Maximized;
                    break;
                case 10:
                    nOptions |= NAMESPACE_VOS(OProcess)::TOption_FullScreen;
                    break;
            }

            BOOL bSync = FALSE;
            if( nArgCount >= 5 )
                bSync = rPar.Get(4)->GetBool();
            if( bSync )
                nOptions |= NAMESPACE_VOS(OProcess)::TOption_Wait;
        }
        NAMESPACE_VOS(OProcess)::TProcessOption eOptions =
            (NAMESPACE_VOS(OProcess)::TProcessOption)nOptions;


        // #72471 Parameter aufbereiten
        std::list<String>::const_iterator iter = aTokenList.begin();
        const String& rStr = *iter;
        ::rtl::OUString aOUStrProg( rStr.GetBuffer(), rStr.Len() );
        String aOUStrProgUNC = getFullPathUNC( aOUStrProg );

        iter++;

        USHORT nParamCount = aTokenList.size() - 1;
        ::rtl::OUString* pArgumentList = NULL;
        //const char** pParamList = NULL;
        if( nParamCount )
        {
            pArgumentList = new ::rtl::OUString[ nParamCount ];
            //pParamList = new const char*[ nParamCount ];
            USHORT iList = 0;
            while( iter != aTokenList.end() )
            {
                const String& rParamStr = (*iter);
                pArgumentList[iList++] = ::rtl::OUString( rParamStr.GetBuffer(), rParamStr.Len() );
                //pParamList[iList++] = (*iter).GetStr();
                iter++;
            }
        }

        //const char* pParams = aParams.Len() ? aParams.GetStr() : 0;
        NAMESPACE_VOS(OProcess)* pApp;
        pApp = new NAMESPACE_VOS(OProcess)( aOUStrProgUNC );
        BOOL bSucc;
        if( nParamCount == 0 )
        {
            bSucc = pApp->execute( eOptions ) == NAMESPACE_VOS(OProcess)::E_None;
        }
        else
        {
            NAMESPACE_VOS(OArgumentList) aArgList( pArgumentList, nParamCount );
            bSucc = pApp->execute( eOptions, aArgList ) == NAMESPACE_VOS(OProcess)::E_None;
        }

        /*
        if( nParamCount == 0 )
            pApp = new NAMESPACE_VOS(OProcess)( pProg );
        else
            pApp = new NAMESPACE_VOS(OProcess)( pProg, pParamList, nParamCount );
        BOOL bSucc = pApp->execute( eOptions ) == NAMESPACE_VOS(OProcess)::E_None;
        */

        delete pApp;
        delete[] pArgumentList;
        if( !bSucc )
            StarBASIC::Error( SbERR_FILE_NOT_FOUND );
        else
            rPar.Get(0)->PutLong( 0 );
    }
}

RTLFUNC(VarType)
{
    if ( rPar.Count() != 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbxDataType eType = rPar.Get(1)->GetType();
        rPar.Get(0)->PutInteger( (INT16)eType );
    }
}

RTLFUNC(TypeName)
{
    static const char* pTypeNames[] =
    {
        "Empty",            // SbxEMPTY
        "Null",             // SbxNULL
        "Integer",          // SbxINTEGER
        "Long",             // SbxLONG
        "Single",           // SbxSINGLE
        "Double",           // SbxDOUBLE
        "Currency",         // SbxCURRENCY
        "Date",             // SbxDATE
        "String",           // SbxSTRING
        "Object",           // SbxOBJECT
        "Error",            // SbxERROR
        "Boolean",          // SbxBOOL
        "Variant",          // SbxVARIANT
        "DataObject",       // SbxDATAOBJECT
        "Unknown Type",     //
        "Unknown Type",     //
        "Char",             // SbxCHAR
        "Byte",             // SbxBYTE
        "UShort",           // SbxUSHORT
        "ULong",            // SbxULONG
        "Long64",           // SbxLONG64
        "ULong64",          // SbxULONG64
        "Int",              // SbxINT
        "UInt",             // SbxUINT
        "Void",             // SbxVOID
        "HResult",          // SbxHRESULT
        "Pointer",          // SbxPOINTER
        "DimArray",         // SbxDIMARRAY
        "CArray",           // SbxCARRAY
        "Userdef",          // SbxUSERDEF
        "Lpstr",            // SbxLPSTR
        "Lpwstr",           // SbxLPWSTR
        "Unknown Type",     // SbxCoreSTRING
        "WString",          // SbxWSTRING
        "WChar",            // SbxWCHAR
        "Int64",            // SbxSALINT64
        "UInt64",           // SbxSALUINT64
    };

    if ( rPar.Count() != 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        SbxDataType eType = rPar.Get(1)->GetType();
        BOOL bIsArray = ( ( eType & SbxARRAY ) != 0 );
        int nPos = ((int)eType) & 0x0FFF;
        USHORT nTypeNameCount = sizeof( pTypeNames ) / sizeof( char* );
        if ( nPos < 0 || nPos >= nTypeNameCount )
            nPos = nTypeNameCount - 1;
        String aRetStr = String::CreateFromAscii( pTypeNames[nPos] );
        if( bIsArray )
            aRetStr.AppendAscii( "()" );
        rPar.Get(0)->PutString( aRetStr );
    }
}

RTLFUNC(Len)
{
    if ( rPar.Count() != 2 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else
    {
        const String& rStr = rPar.Get(1)->GetString();
        rPar.Get(0)->PutLong( (INT32)rStr.Len() );
    }
}

RTLFUNC(DDEInitiate)
{
    // No DDE for "virtual" portal users
    if( needSecurityRestrictions() )
    {
        StarBASIC::Error(SbERR_NOT_IMPLEMENTED);
        return;
    }

    int nArgs = (int)rPar.Count();
    if ( nArgs != 3 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }
    const String& rApp = rPar.Get(1)->GetString();
    const String& rTopic = rPar.Get(2)->GetString();

    SbiDdeControl* pDDE = pINST->GetDdeControl();
    INT16 nChannel;
    SbError nDdeErr = pDDE->Initiate( rApp, rTopic, nChannel );
    if( nDdeErr )
        StarBASIC::Error( nDdeErr );
    else
        rPar.Get(0)->PutInteger( nChannel );
}

RTLFUNC(DDETerminate)
{
    // No DDE for "virtual" portal users
    if( needSecurityRestrictions() )
    {
        StarBASIC::Error(SbERR_NOT_IMPLEMENTED);
        return;
    }

    rPar.Get(0)->PutEmpty();
    int nArgs = (int)rPar.Count();
    if ( nArgs != 2 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }
    INT16 nChannel = rPar.Get(1)->GetInteger();
    SbiDdeControl* pDDE = pINST->GetDdeControl();
    SbError nDdeErr = pDDE->Terminate( nChannel );
    if( nDdeErr )
        StarBASIC::Error( nDdeErr );
}

RTLFUNC(DDETerminateAll)
{
    // No DDE for "virtual" portal users
    if( needSecurityRestrictions() )
    {
        StarBASIC::Error(SbERR_NOT_IMPLEMENTED);
        return;
    }

    rPar.Get(0)->PutEmpty();
    int nArgs = (int)rPar.Count();
    if ( nArgs != 1 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }

    SbiDdeControl* pDDE = pINST->GetDdeControl();
    SbError nDdeErr = pDDE->TerminateAll();
    if( nDdeErr )
        StarBASIC::Error( nDdeErr );

}

RTLFUNC(DDERequest)
{
    // No DDE for "virtual" portal users
    if( needSecurityRestrictions() )
    {
        StarBASIC::Error(SbERR_NOT_IMPLEMENTED);
        return;
    }

    int nArgs = (int)rPar.Count();
    if ( nArgs != 3 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }
    INT16 nChannel = rPar.Get(1)->GetInteger();
    const String& rItem = rPar.Get(2)->GetString();
    SbiDdeControl* pDDE = pINST->GetDdeControl();
    String aResult;
    SbError nDdeErr = pDDE->Request( nChannel, rItem, aResult );
    if( nDdeErr )
        StarBASIC::Error( nDdeErr );
    else
        rPar.Get(0)->PutString( aResult );
}

RTLFUNC(DDEExecute)
{
    // No DDE for "virtual" portal users
    if( needSecurityRestrictions() )
    {
        StarBASIC::Error(SbERR_NOT_IMPLEMENTED);
        return;
    }

    rPar.Get(0)->PutEmpty();
    int nArgs = (int)rPar.Count();
    if ( nArgs != 3 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }
    INT16 nChannel = rPar.Get(1)->GetInteger();
    const String& rCommand = rPar.Get(2)->GetString();
    SbiDdeControl* pDDE = pINST->GetDdeControl();
    SbError nDdeErr = pDDE->Execute( nChannel, rCommand );
    if( nDdeErr )
        StarBASIC::Error( nDdeErr );
}

RTLFUNC(DDEPoke)
{
    // No DDE for "virtual" portal users
    if( needSecurityRestrictions() )
    {
        StarBASIC::Error(SbERR_NOT_IMPLEMENTED);
        return;
    }

    rPar.Get(0)->PutEmpty();
    int nArgs = (int)rPar.Count();
    if ( nArgs != 4 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }
    INT16 nChannel = rPar.Get(1)->GetInteger();
    const String& rItem = rPar.Get(2)->GetString();
    const String& rData = rPar.Get(3)->GetString();
    SbiDdeControl* pDDE = pINST->GetDdeControl();
    SbError nDdeErr = pDDE->Poke( nChannel, rItem, rData );
    if( nDdeErr )
        StarBASIC::Error( nDdeErr );
}


RTLFUNC(FreeFile)
{
    if ( rPar.Count() != 1 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }
    SbiIoSystem* pIO = pINST->GetIoSystem();
    short nChannel = 1;
    while( nChannel < CHANNELS )
    {
        SbiStream* pStrm = pIO->GetStream( nChannel );
        if( !pStrm )
        {
            rPar.Get(0)->PutInteger( nChannel );
            return;
        }
        nChannel++;
    }
    StarBASIC::Error( SbERR_TOO_MANY_FILES );
}

RTLFUNC(LBound)
{
    USHORT nParCount = rPar.Count();
    if ( nParCount != 3 && nParCount != 2 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }
    SbxBase* pParObj = rPar.Get(1)->GetObject();
    SbxDimArray* pArr = PTR_CAST(SbxDimArray,pParObj);
    if( pArr )
    {
        INT32 nLower, nUpper;
        short nDim = (nParCount == 3) ? (short)rPar.Get(2)->GetInteger() : 1;
        if( !pArr->GetDim32( nDim, nLower, nUpper ) )
            StarBASIC::Error( SbERR_OUT_OF_RANGE );
        else
            rPar.Get(0)->PutLong( nLower );
    }
    else
        StarBASIC::Error( SbERR_MUST_HAVE_DIMS );
}

RTLFUNC(UBound)
{
    USHORT nParCount = rPar.Count();
    if ( nParCount != 3 && nParCount != 2 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }

    SbxBase* pParObj = rPar.Get(1)->GetObject();
    SbxDimArray* pArr = PTR_CAST(SbxDimArray,pParObj);
    if( pArr )
    {
        INT32 nLower, nUpper;
        short nDim = (nParCount == 3) ? (short)rPar.Get(2)->GetInteger() : 1;
        if( !pArr->GetDim32( nDim, nLower, nUpper ) )
            StarBASIC::Error( SbERR_OUT_OF_RANGE );
        else
            rPar.Get(0)->PutLong( nUpper );
    }
    else
        StarBASIC::Error( SbERR_MUST_HAVE_DIMS );
}

RTLFUNC(RGB)
{
    if ( rPar.Count() != 4 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }

    ULONG nRed   = rPar.Get(1)->GetInteger() & 0xFF;
    ULONG nGreen = rPar.Get(2)->GetInteger() & 0xFF;
    ULONG nBlue  = rPar.Get(3)->GetInteger() & 0xFF;
    ULONG nRGB   = (nRed << 16) | (nGreen << 8) | nBlue;
    rPar.Get(0)->PutLong( nRGB );
}

RTLFUNC(QBColor)
{
    if ( rPar.Count() != 2 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }

    Color aCol( (ColorName)rPar.Get(1)->GetInteger() );

    ULONG nRed   = aCol.GetRed() >> 8;
    ULONG nGreen = aCol.GetGreen() >> 8;
    ULONG nBlue  = aCol.GetBlue() >> 8;
    ULONG nRGB   = (nRed << 16) | (nGreen << 8) | nBlue;
    rPar.Get(0)->PutLong( nRGB );
}


RTLFUNC(StrConv)
{
    DBG_ASSERT(0,"StrConv:Not implemented");
//  if ( rPar.Count() != 3 )
//  {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
//      return;
//  }
}

RTLFUNC(Beep)
{
    if ( rPar.Count() != 1 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }
    Sound::Beep();
}

RTLFUNC(Load)
{
    if( rPar.Count() != 2 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }

    // Diesen Call einfach an das Object weiterreichen
    SbxBase* pObj = (SbxObject*)rPar.Get(1)->GetObject();
    if( pObj && pObj->IsA( TYPE( SbxObject ) ) )
    {
        SbxVariable* pVar = ((SbxObject*)pObj)->
            Find( String( RTL_CONSTASCII_USTRINGPARAM("Load") ), SbxCLASS_METHOD );
        if( pVar )
            pVar->GetInteger();
    }
}

RTLFUNC(Unload)
{
    rPar.Get(0)->PutEmpty();
    if( rPar.Count() != 2 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }

    // Diesen Call einfach an das Object weitereichen
    SbxBase* pObj = (SbxObject*)rPar.Get(1)->GetObject();
    if( pObj && pObj->IsA( TYPE( SbxObject ) ) )
    {
        SbxVariable* pVar = ((SbxObject*)pObj)->
            Find( String( RTL_CONSTASCII_USTRINGPARAM("Unload") ), SbxCLASS_METHOD );
        if( pVar )
            pVar->GetInteger();
    }
}

RTLFUNC(LoadPicture)
{
    if( rPar.Count() != 2 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }

    String aFileURL = getFullPath( rPar.Get(1)->GetString() );
    SvStream* pStream = utl::UcbStreamHelper::CreateStream( aFileURL, STREAM_READ );
    if( pStream != NULL )
    {
        Bitmap aBmp;
        *pStream >> aBmp;
        Graphic aGraphic( aBmp );

        SbxObjectRef xRef = new SbStdPicture;
        ((SbStdPicture*)(SbxObject*)xRef)->SetGraphic( aGraphic );
        rPar.Get(0)->PutObject( xRef );
    }
    delete pStream;
}

RTLFUNC(SavePicture)
{
    rPar.Get(0)->PutEmpty();
    if( rPar.Count() != 3 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }

    SbxBase* pObj = (SbxObject*)rPar.Get(1)->GetObject();
    if( pObj->IsA( TYPE( SbStdPicture ) ) )
    {
        SvFileStream aOStream( rPar.Get(2)->GetString(), STREAM_WRITE | STREAM_TRUNC );
        Graphic aGraphic = ((SbStdPicture*)pObj)->GetGraphic();
        aOStream << aGraphic;
    }
}


//-----------------------------------------------------------------------------------------
/*
class SbiAboutStarBasicDlg : public ModalDialog
{
    OKButton    aOkButton;
    Control     aCtrl;

public:
        SbiAboutStarBasicDlg();
};

SbiAboutStarBasicDlg::SbiAboutStarBasicDlg() :
    ModalDialog( GetpApp()->GetAppWindow(), BasicResId( RID_BASIC_START ) ),
    aOkButton( this, BasicResId( 1 ) ),
    aCtrl( this, BasicResId( 1 ) )
{
    FreeResource();
}
*/
//-----------------------------------------------------------------------------------------

RTLFUNC(AboutStarBasic)
{
    /*
    String aName;
    if( rPar.Count() >= 2 )
    {
        aName = rPar.Get(1)->GetString();
    }

    SbiAboutStarBasicDlg* pDlg = new SbiAboutStarBasicDlg;
    pDlg->Execute();
    delete pDlg;
    */
}

// MsgBox( msg [,type[,title]] )

RTLFUNC(MsgBox)
{
    static const WinBits nStyleMap[] =
    {
        WB_OK,              // MB_OK
        WB_OK_CANCEL,       // MB_OKCANCEL
        WB_RETRY_CANCEL,    // MB_ABORTRETRYIGNORE
        WB_YES_NO_CANCEL,   // MB_YESNOCANCEL
        WB_YES_NO,          // MB_YESNO
        WB_RETRY_CANCEL     // MB_RETRYCANCEL
    };
    static const INT16 nButtonMap[] =
    {
        2, // #define RET_CANCEL FALSE
        1, // #define RET_OK     TRUE
        6, // #define RET_YES    2
        7, // #define RET_NO     3
        4  // #define RET_RETRY  4
    };


    USHORT nArgCount = (USHORT)rPar.Count();
    if( nArgCount < 2 || nArgCount > 4 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }
    WinBits nWinBits;
    WinBits nType = 0; // MB_OK
    if( nArgCount >= 3 )
        nType = (WinBits)rPar.Get(2)->GetInteger();
    WinBits nStyle = nType;
    nStyle &= 15; // Bits 4-16 loeschen
    if( nStyle > 5 )
        nStyle = 0;

    nWinBits = nStyleMap[ nStyle ];
    if( nType & 4096 )
        nWinBits |= WB_SYSMODAL;

    WinBits nWinDefBits;
    nWinDefBits = (WB_DEF_OK | WB_DEF_RETRY | WB_DEF_YES);
    if( nType & 256 )
    {
        if( nStyle == 5 || nStyle == 2)
            nWinDefBits = WB_DEF_CANCEL;
        else
            nWinDefBits = (WB_DEF_CANCEL | WB_DEF_RETRY | WB_DEF_NO);
    }
    if( nType & 512 )
        nWinDefBits = WB_DEF_CANCEL;
    nWinBits |= nWinDefBits;

    String aMsg = rPar.Get(1)->GetString();
    String aTitle;
    if( nArgCount == 4 )
        aTitle = rPar.Get(3)->GetString();
    else
        aTitle = GetpApp()->GetAppName();

    nType &= (16+32+64);
    MessBox* pBox = 0;
    Window* pParent = GetpApp()->GetDefModalDialogParent();
    switch( nType )
    {
        case 16:
            pBox = new ErrorBox( pParent, nWinBits, aMsg );
            break;
        case 32:
            pBox = new QueryBox( pParent, nWinBits, aMsg );
            break;
        case 48:
            pBox = new WarningBox( pParent, nWinBits, aMsg );
            break;
        case 64:
            pBox = new InfoBox( pParent, aMsg );
            break;
        default:
            pBox = new MessBox( pParent, nWinBits, aTitle, aMsg );
    }
    pBox->SetText( aTitle );
    USHORT nRet = (USHORT)pBox->Execute();
    if( nRet == TRUE )
        nRet = 1;
    rPar.Get(0)->PutInteger( nButtonMap[ nRet ] );
    delete pBox;
}

RTLFUNC(SetAttr) // JSM
{
    rPar.Get(0)->PutEmpty();
    if ( rPar.Count() == 3 )
    {
        String aStr = rPar.Get(1)->GetString();
        INT16 nFlags = rPar.Get(2)->GetInteger();

        // <-- UCB
        if( hasUno() )
        {
            Reference< XSimpleFileAccess3 > xSFI = getFileAccess();
            if( xSFI.is() )
            {
                try
                {
                    sal_Bool bReadOnly = (nFlags & 0x0001) != 0; // ATTR_READONLY
                    xSFI->setReadOnly( aStr, bReadOnly );
                    sal_Bool bHidden   = (nFlags & 0x0002) != 0; // ATTR_HIDDEN
                    xSFI->setHidden( aStr, bHidden );
                }
                catch( Exception & )
                {
                    StarBASIC::Error( ERRCODE_IO_GENERAL );
                }
            }
        }
        else
        // --> UCB
        {
#ifdef _OLD_FILE_IMPL
            // #57064 Bei virtuellen URLs den Real-Path extrahieren
            DirEntry aEntry( aStr );
            String aFile = aEntry.GetFull();
    #ifdef WIN
            int nErr = _dos_setfileattr( aFile.GetStr(),(unsigned ) nFlags );
            if ( nErr )
            {
                if (errno == EACCES)
                    StarBASIC::Error( SbERR_ACCESS_DENIED );
                else
                    StarBASIC::Error( SbERR_FILE_NOT_FOUND );
            }
    #endif
            ByteString aByteFile( aFile, gsl_getSystemTextEncoding() );
    #ifdef WNT
            if (!SetFileAttributes (aByteFile.GetBuffer(),(DWORD)nFlags))
                StarBASIC::Error(SbERR_FILE_NOT_FOUND);
    #endif
    #ifdef OS2
            FILESTATUS3 aFileStatus;
            APIRET rc = DosQueryPathInfo(aByteFile.GetBuffer(),1,
                                         &aFileStatus,sizeof(FILESTATUS3));
            if (!rc)
            {
                if (aFileStatus.attrFile != nFlags)
                {
                    aFileStatus.attrFile = nFlags;
                    rc = DosSetPathInfo(aFile.GetStr(),1,
                                        &aFileStatus,sizeof(FILESTATUS3),0);
                    if (rc)
                        StarBASIC::Error( SbERR_FILE_NOT_FOUND );
                }
            }
            else
                StarBASIC::Error( SbERR_FILE_NOT_FOUND );
    #endif
#else
            sal_Bool bReadOnly = (nFlags & 0x0001) != 0; // ATTR_READONLY
            sal_uInt64 nAttrs = bReadOnly ? Attribute_ReadOnly : 0;
            String aPath = getFullPathUNC( rPar.Get(1)->GetString() );
#endif
        }
    }
    else
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
}

RTLFUNC(Reset)  // JSM
{
    SbiIoSystem* pIO = pINST->GetIoSystem();
    if (pIO)
        pIO->CloseAll();
}

RTLFUNC(DumpAllObjects)
{
    USHORT nArgCount = (USHORT)rPar.Count();
    if( nArgCount < 2 || nArgCount > 3 )
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
    else if( !pBasic )
        StarBASIC::Error( SbERR_INTERNAL_ERROR );
    else
    {
        SbxObject* p = pBasic;
        while( p->GetParent() )
            p = p->GetParent();
        SvFileStream aStrm( rPar.Get( 1 )->GetString(),
                            STREAM_WRITE | STREAM_TRUNC );
        p->Dump( aStrm, rPar.Get( 2 )->GetBool() );
        aStrm.Close();
        if( aStrm.GetError() != SVSTREAM_OK )
            StarBASIC::Error( SbERR_IO_ERROR );
    }
}


RTLFUNC(FileExists)
{
    if ( rPar.Count() == 2 )
    {
        String aStr = rPar.Get(1)->GetString();
        BOOL bExists = FALSE;

        // <-- UCB
        if( hasUno() )
        {
            Reference< XSimpleFileAccess3 > xSFI = getFileAccess();
            if( xSFI.is() )
            {
                try
                {
                    bExists = xSFI->exists( aStr );
                }
                catch( Exception & )
                {
                    StarBASIC::Error( ERRCODE_IO_GENERAL );
                }
            }
        }
        else
        // --> UCB
        {
#ifdef _OLD_FILE_IMPL
            DirEntry aEntry( aStr );
            bExists = aEntry.Exists();
#else
            DirectoryItem aItem;
            FileBase::RC nRet = DirectoryItem::get( getFullPathUNC( aStr ), aItem );
            bExists = (nRet == FileBase::E_None);
#endif
        }
        rPar.Get(0)->PutBool( bExists );
    }
    else
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
}

#ifndef PRODUCT
// For debugging only
void dbg_SaveDisassembly( SbModule* pModule )
{
    Reference< XSimpleFileAccess3 > xSFI;
    Reference< XTextOutputStream > xTextOut;
    Reference< XOutputStream > xOut;
    Reference< XMultiServiceFactory > xSMgr = getProcessServiceFactory();
    if( xSMgr.is() )
    {
        Reference< XSimpleFileAccess3 > xSFI = Reference< XSimpleFileAccess3 >( xSMgr->createInstance
            ( OUString::createFromAscii( "com.sun.star.ucb.SimpleFileAccess" ) ), UNO_QUERY );
        if( xSFI.is() )
        {
            String aFile( RTL_CONSTASCII_USTRINGPARAM("file:///d:/BasicAsm.txt") );
            if( xSFI->exists( aFile ) )
                xSFI->kill( aFile );
            xOut = xSFI->openFileWrite( aFile );
            Reference< XInterface > x = xSMgr->createInstance( OUString::createFromAscii( "com.sun.star.io.TextOutputStream" ) );
            Reference< XActiveDataSource > xADS( x, UNO_QUERY );
            xADS->setOutputStream( xOut );
            xTextOut = Reference< XTextOutputStream >( x, UNO_QUERY );
        }
    }

    if( xTextOut.is() )
    {
        String aDisassemblyStr;
        pModule->Disassemble( aDisassemblyStr );
        xTextOut->writeString( aDisassemblyStr );
    }
    xOut->closeOutput();
}
#endif
