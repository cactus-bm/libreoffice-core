/*************************************************************************
 *
 *  $RCSfile: sm.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: vg $ $Date: 2003-04-15 16:08:43 $
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
#include <string.h>
#include <unistd.h>

// [ed] 6/15/02 Use the poll replacement on OS X
#ifdef MACOSX
#include <poll.h>
#else
#include <sys/poll.h>
#endif
#include <stdio.h>

#include <osl/process.h>
#include <osl/security.h>

#include <prex.h>
#include <X11/Xatom.h>
#include <postx.h>

#ifndef _VCL_SM_HXX
#include <sm.hxx>
#endif
#ifndef _SV_SALDATA_HXX
#include <saldata.hxx>
#endif
#ifndef _SV_SALDISP_HXX
#include <saldisp.hxx>
#endif
#ifndef _SV_SALFRAME_HXX
#include <salframe.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <svapp.hxx>
#endif

#define USE_SM_EXTENSION

extern "C" void SAL_CALL ICEConnectionWorker( void* );

class ICEConnectionObserver
{
    friend void SAL_CALL ICEConnectionWorker(void*);
    static BOOL bIsWatching;
    static void ICEWatchProc( IceConn connection, IcePointer client_data,
                              Bool opening, IcePointer* watch_data );

    static struct pollfd* pFilehandles;
    static IceConn* pConnections;
    static int nConnections;
    static oslMutex ICEMutex;
    static oslThread ICEThread;
public:

    static void activate();
    static void deactivate();
    static void lock();
    static void unlock();
};


BOOL ICEConnectionObserver::bIsWatching = FALSE;
SmcConn SessionManagerClient::aSmcConnection = NULL;
ByteString SessionManagerClient::aClientID;
struct pollfd* ICEConnectionObserver::pFilehandles = NULL;
IceConn* ICEConnectionObserver::pConnections = NULL;
int ICEConnectionObserver::nConnections = 0;
oslMutex ICEConnectionObserver::ICEMutex = NULL;
oslThread ICEConnectionObserver::ICEThread = NULL;

static SmProp*  pSmProps = NULL;
static SmProp** ppSmProps = NULL;
static int      nSmProps = 0;

static void BuildSmPropertyList()
{
    if( ! pSmProps )
    {
        ByteString aExec( SessionManagerClient::getExecName(), osl_getThreadTextEncoding() );

        nSmProps = 4;
        pSmProps = new SmProp[ nSmProps ];

        pSmProps[ 0 ].name      = SmCloneCommand;
        pSmProps[ 0 ].type      = SmLISTofARRAY8;
        pSmProps[ 0 ].num_vals  = 1;
        pSmProps[ 0 ].vals      = new SmPropValue;
        pSmProps[ 0 ].vals->length  = aExec.Len()+1;
        pSmProps[ 0 ].vals->value   = strdup( aExec.GetBuffer() );

        pSmProps[ 1 ].name      = SmProgram;
        pSmProps[ 1 ].type      = SmARRAY8;
        pSmProps[ 1 ].num_vals  = 1;
        pSmProps[ 1 ].vals      = new SmPropValue;
        pSmProps[ 1 ].vals->length  = aExec.Len()+1;
        pSmProps[ 1 ].vals->value   = strdup( aExec.GetBuffer() );

        pSmProps[ 2 ].name      = SmRestartCommand;
        pSmProps[ 2 ].type      = SmLISTofARRAY8;
        pSmProps[ 2 ].num_vals  = 2;
        pSmProps[ 2 ].vals      = new SmPropValue[2];
        pSmProps[ 2 ].vals[0].length    = aExec.Len()+1;
        pSmProps[ 2 ].vals[0].value     = strdup( aExec.GetBuffer() );
        ByteString aRestartOption( "-session=" );
        aRestartOption.Append( SessionManagerClient::getSessionID() );
        pSmProps[ 2 ].vals[1].length    = aRestartOption.Len()+1;
        pSmProps[ 2 ].vals[1].value     = strdup( aRestartOption.GetBuffer() );

        rtl::OUString aUserName;
        rtl::OString aUser;
        oslSecurity aSec = osl_getCurrentSecurity();
        if( aSec )
        {
            osl_getUserName( aSec, &aUserName.pData );
            aUser = rtl::OUStringToOString( aUserName, osl_getThreadTextEncoding() );
            osl_freeSecurityHandle( aSec );
        }

        pSmProps[ 3 ].name      = SmUserID;
        pSmProps[ 3 ].type      = SmARRAY8;
        pSmProps[ 3 ].num_vals  = 1;
        pSmProps[ 3 ].vals      = new SmPropValue;
        pSmProps[ 3 ].vals->value   = strdup( aUser.getStr() );
        pSmProps[ 3 ].vals->length  = strlen( (char *)pSmProps[ 3 ].vals->value )+1;

        ppSmProps = new SmProp*[ nSmProps ];
        for( int i = 0; i < nSmProps; i++ )
            ppSmProps[ i ] = &pSmProps[i];
    }
}


void SessionManagerClient::SaveYourselfProc(
    SmcConn connection,
    SmPointer client_data,
    int save_type,
    Bool shutdown,
    int interact_style,
    Bool fast
    )
{
#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "Session: save yourself, save_type = %s, shutdown = %s, interact_style = %s, fast = %s\n",
             save_type == SmSaveLocal ? "SmcSaveLocal" :
             ( save_type == SmSaveGlobal ? "SmcSaveGlobal" :
               ( save_type == SmSaveBoth ? "SmcSaveBoth" : "<unknown>" ) ),
             shutdown ? "true" : "false",
             interact_style == SmInteractStyleNone ? "SmInteractStyleNone" :
             ( interact_style == SmInteractStyleErrors ? "SmInteractStyleErrors" :
               ( interact_style == SmInteractStyleAny ? "SmInteractStyleAny" : "<unknown>" ) ),
             false ? "true" : "false"
             );
#endif
    BuildSmPropertyList();
#ifdef USE_SM_EXTENSION
    SmcSetProperties( aSmcConnection, nSmProps, ppSmProps );
    SmcSaveYourselfDone( aSmcConnection, True );
#endif
}

IMPL_STATIC_LINK( SessionManagerClient, ShutDownHdl, void*, pDummy )
{
#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, GetSalData()->pFirstFrame_ ? "shutdown on first frame\n" : "shutdown event but no frame\n" );
#endif
    if( GetSalData()->pFirstFrame_ )
    {
        GetSalData()->pFirstFrame_->maFrameData.ShutDown();
    }
    return 0;
}

void SessionManagerClient::DieProc(
    SmcConn connection,
    SmPointer client_data
    )
{
#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "Session: die\n" );
#endif
#ifdef USE_SM_EXTENSION
    SmcCloseConnection( connection, 0, NULL );
#endif
    if( connection == aSmcConnection )
        aSmcConnection = NULL;
    Application::PostUserEvent( STATIC_LINK( NULL, SessionManagerClient, ShutDownHdl ) );
}

void SessionManagerClient::SaveCompleteProc(
    SmcConn connection,
    SmPointer client_data
    )
{
#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "Session: save complete\n" );
#endif
}

void SessionManagerClient::ShutdownCanceledProc(
    SmcConn connection,
    SmPointer client_data )
{
#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "Session: shutdown canceled\n" );
#endif
}

void SessionManagerClient::open()
{
    static SmcCallbacks aCallbacks;

#ifdef USE_SM_EXTENSION
    // this is the way Xt does it, so we can too
    if( ! aSmcConnection && getenv( "SESSION_MANAGER" ) )
    {
        char aErrBuf[1024];
        ICEConnectionObserver::activate();
        ICEConnectionObserver::lock();

        char* pClientID = NULL;
        const ByteString& rPrevId( getPreviousSessionID() );

        aCallbacks.save_yourself.callback           = SaveYourselfProc;
        aCallbacks.save_yourself.client_data        = NULL;
        aCallbacks.die.callback                     = DieProc;
        aCallbacks.die.client_data                  = NULL;
        aCallbacks.save_complete.callback           = SaveCompleteProc;
        aCallbacks.save_complete.client_data        = NULL;
        aCallbacks.shutdown_cancelled.callback      = ShutdownCanceledProc;
        aCallbacks.shutdown_cancelled.client_data   = NULL;
        aSmcConnection = SmcOpenConnection( NULL,
                                            NULL,
                                            SmProtoMajor,
                                            SmProtoMinor,
                                            SmcSaveYourselfProcMask         |
                                            SmcDieProcMask                  |
                                            SmcSaveCompleteProcMask         |
                                            SmcShutdownCancelledProcMask    ,
                                            &aCallbacks,
                                            rPrevId.Len() ? const_cast<char*>(rPrevId.GetBuffer()) : NULL,
                                            &pClientID,
                                            sizeof( aErrBuf ),
                                            aErrBuf );
#if (OSL_DEBUG_LEVEL > 1)  || defined DBG_UTIL
        if( ! aSmcConnection )
            fprintf( stderr, "SmcOpenConnection failed: %s\n", aErrBuf );
#if OSL_DEBUG_LEVEL > 1
        else
            fprintf( stderr, "SmcOpenConnection succeeded, client ID is \"%s\"\n", pClientID );
#endif
#endif
        aClientID = ByteString( pClientID );
        free( pClientID );
        pClientID = NULL;
        ICEConnectionObserver::unlock();

        SalDisplay* pDisp = GetSalData()->GetDefDisp();
        if( pDisp->GetDrawable() && aClientID.Len() )
        {
            XChangeProperty( pDisp->GetDisplay(),
                             pDisp->GetDrawable(),
                             XInternAtom( pDisp->GetDisplay(), "SM_CLIENT_ID", False ),
                             XA_STRING,
                             8,
                             PropModeReplace,
                             (unsigned char*)aClientID.GetBuffer(),
                             aClientID.Len()
                             );
        }
    }
#if OSL_DEBUG_LEVEL > 1
    else if( aSmcConnection )
        fprintf( stderr, "no SESSION_MANAGER\n" );
#endif
#endif
}

void SessionManagerClient::close()
{
    if( aSmcConnection )
    {
#ifdef USE_SM_EXTENSION
        SmcCloseConnection( aSmcConnection, 0, NULL );
#if OSL_DEBUG_LEVEL > 1
        fprintf( stderr, "SmcConnection closed\n" );
#endif
#endif
        aSmcConnection = NULL;
        ICEConnectionObserver::deactivate();
    }
}


String SessionManagerClient::getExecName()
{
    rtl::OUString aExec, aSysExec;
    osl_getExecutableFile( &aExec.pData );
    osl_getSystemPathFromFileURL( aExec.pData, &aSysExec.pData );

    int nPos = aSysExec.indexOf( rtl::OUString::createFromAscii( ".bin" ) );
    if( nPos != -1 )
        aSysExec = aSysExec.copy( 0, nPos );
    return aSysExec;
}


const ByteString& SessionManagerClient::getPreviousSessionID()
{
    static ByteString aPrevId;

    int nCommands = osl_getCommandArgCount();
    for( int i = 0; i < nCommands; i++ )
    {
        ::rtl::OUString aArg;
        osl_getCommandArg( i, &aArg.pData );
        if( aArg.compareToAscii( "-session=", 9 ) == 0 )
        {
            aPrevId = ByteString( ::rtl::OUStringToOString( aArg.copy( 9 ), osl_getThreadTextEncoding() ) );
            break;
        }
    }
#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "previous ID = \"%s\"\n", aPrevId.GetBuffer() );
#endif
    return aPrevId;
}

void ICEConnectionObserver::lock()
{
    osl_acquireMutex( ICEMutex );
}

void ICEConnectionObserver::unlock()
{
    osl_releaseMutex( ICEMutex );
}

void ICEConnectionObserver::activate()
{
    if( ! bIsWatching )
    {
        ICEMutex = osl_createMutex();
        bIsWatching = TRUE;
#ifdef USE_SM_EXTENSION
        IceAddConnectionWatch( ICEWatchProc, NULL );
#endif
    }
}

void ICEConnectionObserver::deactivate()
{
    if( bIsWatching )
    {
        bIsWatching = FALSE;
#ifdef USE_SM_EXTENSION
        IceRemoveConnectionWatch( ICEWatchProc, NULL );
#endif
        if( ICEThread )
        {
            osl_terminateThread( ICEThread );
            osl_destroyThread( ICEThread );
            ICEThread = NULL;
        }
        osl_destroyMutex( ICEMutex );
    }
}

void ICEConnectionWorker( void* pData )
{
#ifdef USE_SM_EXTENSION
    while( ICEConnectionObserver::nConnections )
    {
        osl_acquireMutex( ICEConnectionObserver::ICEMutex );
        int nRet = poll( ICEConnectionObserver::pFilehandles,
                         ICEConnectionObserver::nConnections,
                         400 );
        if( nRet > 0 )
        {
#if OSL_DEBUG_LEVEL > 1
            fprintf( stderr, "IceProcessMessages\n" );
#endif
            Bool bReply;
            for( int i = 0; i < ICEConnectionObserver::nConnections; i++ )
                if( ICEConnectionObserver::pFilehandles[i].revents & POLLIN )
                    IceProcessMessages( ICEConnectionObserver::pConnections[i], NULL, &bReply );
        }
        osl_releaseMutex( ICEConnectionObserver::ICEMutex );
        osl_yieldThread();
    }
#endif
}

void ICEConnectionObserver::ICEWatchProc(
    IceConn connection,
    IcePointer client_data,
    Bool opening,
    IcePointer* watch_data
    )
{
#ifdef USE_SM_EXTENSION
    if( opening )
    {
        osl_acquireMutex( ICEMutex );
        int fd = IceConnectionNumber( connection );
        nConnections++;
        pConnections = (IceConn*)rtl_reallocateMemory( pConnections, sizeof( IceConn )*nConnections );
        pFilehandles = (struct pollfd*)rtl_reallocateMemory( pFilehandles, sizeof( struct pollfd )*nConnections );
        pConnections[ nConnections-1 ] = connection;
        pFilehandles[ nConnections-1 ].fd       = fd;
        pFilehandles[ nConnections-1 ].events   = POLLIN;
        osl_releaseMutex( ICEMutex );
        if( nConnections == 1 )
            ICEThread = osl_createThread( ICEConnectionWorker, NULL );
    }
    else
    {
        osl_acquireMutex( ICEMutex );
        for( int i = 0; i < nConnections; i++ )
        {
            if( pConnections[i] == connection )
            {
                if( i < nConnections-1 )
                {
                    rtl_moveMemory( pConnections+i, pConnections+i+1, sizeof( IceConn )*(nConnections-i-1) );
                    rtl_moveMemory( pFilehandles+i, pFilehandles+i+1, sizeof( struct pollfd )*(nConnections-i-1) );
                }
                nConnections--;
                pConnections = (IceConn*)rtl_reallocateMemory( pConnections, sizeof( IceConn )*nConnections );
                pFilehandles = (struct pollfd*)rtl_reallocateMemory( pFilehandles, sizeof( struct pollfd )*nConnections );
                break;
            }
        }
        osl_releaseMutex( ICEMutex );
        if( nConnections == 0 )
        {
            osl_terminateThread( ICEThread );
            osl_destroyThread( ICEThread );
            ICEThread = NULL;
        }
    }
#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "ICE connection on %d %s\n",
             IceConnectionNumber( connection ),
             opening ? "inserted" : "removed" );
    fprintf( stderr, "Display connection is %d\n", ConnectionNumber( GetSalData()->GetDefDisp()->GetDisplay() ) );
#endif
#endif
}
