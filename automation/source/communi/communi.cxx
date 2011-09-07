/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#include "precompiled_automation.hxx"
#include <stdio.h>
#if OSL_DEBUG_LEVEL > 1
#define DEBUGPRINTF(x) { printf(x); fflush( stdout ); }
#else
#define DEBUGPRINTF(x)
#endif
#include <tools/debug.hxx>
#include <vcl/svapp.hxx>
#include <osl/socket.hxx>
#include <tools/stream.hxx>
#include <vcl/timer.hxx>
#include <tools/fsys.hxx>

#include <automation/communi.hxx>


/*  The delete below has been removed to make the destructor protected.
    The method isn't used anyway.
//              delete *((AE*)pData+n);
*/

#undef  SV_IMPL_PTRARR_SORT
#define SV_IMPL_PTRARR_SORT( nm,AE )\
_SV_IMPL_SORTAR_ALG( nm,AE )\
    void nm::DeleteAndDestroy( sal_uInt16 nP, sal_uInt16 nL ) { \
        if( nL ) {\
            DBG_ASSERT( nP < nA && nP + nL <= nA, "ERR_VAR_DEL" );\
            for( sal_uInt16 n=nP; n < nP + nL; n++ ) \
                OSL_FAIL("Das Element der Liste wurde nicht gel�scht"); \
            SvPtrarr::Remove( nP, nL ); \
        } \
    } \
_SV_SEEK_PTR( nm, AE )




SV_IMPL_PTRARR_SORT( CommunicationLinkList, CommunicationLink* );

osl::Mutex *pMPostUserEvent=NULL;       // necessary because not threadproof

CommunicationLinkViaSocket::CommunicationLinkViaSocket( CommunicationManager *pMan, osl::StreamSocket* pSocket )
: SimpleCommunicationLinkViaSocket( pMan, pSocket )
, nConnectionClosedEventId( 0 )
, nDataReceivedEventId( 0 )
, bShutdownStarted( sal_False )
, bDestroying( sal_False )
{
    SetPutDataReceivedHdl(LINK( this, CommunicationLinkViaSocket, PutDataReceivedHdl ));
    if ( !pMPostUserEvent )
        pMPostUserEvent = new osl::Mutex;
    // this is necassary to prevent the running thread from sending the close event
    // before the open event has been sent.
       StartCallback();

    create();
}

CommunicationLinkViaSocket::~CommunicationLinkViaSocket()
{
    bDestroying = sal_True;
    StopCommunication();
    while ( nConnectionClosedEventId || nDataReceivedEventId )
        GetpApp()->Yield();
    {
        osl::MutexGuard aGuard( aMConnectionClosed );
        if ( nConnectionClosedEventId )
        {
            GetpApp()->RemoveUserEvent( nConnectionClosedEventId );
            nConnectionClosedEventId = 0;
            INFO_MSG( CByteString("Event gel�scht"),
                CByteString( "ConnectionClosedEvent aus Queue gel�scht"),
                CM_MISC, NULL );
        }
    }
    {
        osl::MutexGuard aGuard( aMDataReceived );
        if ( nDataReceivedEventId )
        {
            GetpApp()->RemoveUserEvent( nDataReceivedEventId );
            nDataReceivedEventId = 0;
            delete GetServiceData();
            INFO_MSG( CByteString("Event gel�scht"),
                CByteString( "DataReceivedEvent aus Queue gel�scht"),
                CM_MISC, NULL );
        }
    }
}

sal_Bool CommunicationLinkViaSocket::ShutdownCommunication()
{
    if ( isRunning() )
    {

        terminate();
        if ( GetStreamSocket() )
            GetStreamSocket()->shutdown();

        if ( GetStreamSocket() )
            GetStreamSocket()->close();

        resume();

        join();

        osl::StreamSocket* pTempSocket = GetStreamSocket();
        SetStreamSocket( NULL );
        delete pTempSocket;

//      ConnectionClosed();     is being called at the end of a thread

    }
    else
    {
        join();
    }

    return sal_True;
}

sal_Bool CommunicationLinkViaSocket::StopCommunication()
{
    if ( !bShutdownStarted )
    {
        return SimpleCommunicationLinkViaSocket::StopCommunication();
    }
    else
    {
        WaitForShutdown();
        return sal_True;
    }
}


IMPL_LINK( CommunicationLinkViaSocket, ShutdownLink, void*, EMPTYARG )
{
    if ( !IsCommunicationError() )
        ShutdownCommunication();
    return 0;
}


void CommunicationLinkViaSocket::WaitForShutdown()
{
    if ( !bShutdownStarted )
    {
        aShutdownTimer.SetTimeout( 30000 );     // Should be 30 Seconds
        aShutdownTimer.SetTimeoutHdl( LINK( this, CommunicationLinkViaSocket, ShutdownLink ) );
        aShutdownTimer.Start();
        bShutdownStarted = sal_True;
    }
    if ( bDestroying )
    {
        while ( pMyManager && aShutdownTimer.IsActive() )
        {
            if ( IsCommunicationError() )
                return;
            GetpApp()->Yield();
        }
        ShutdownCommunication();
    }
}

sal_Bool CommunicationLinkViaSocket::IsCommunicationError()
{
    return !isRunning() || SimpleCommunicationLinkViaSocket::IsCommunicationError();
}

void CommunicationLinkViaSocket::run()
{
    sal_Bool bWasError = sal_False;
    while ( schedule() && !bWasError && GetStreamSocket() )
    {
        if ( bWasError |= !DoReceiveDataStream() )
            continue;

        TimeValue sNochEins = {0, 1000000};
        while ( schedule() && bIsInsideCallback )
            wait( sNochEins );
        SetNewPacketAsCurrent();
        StartCallback();
        {
            osl::MutexGuard aGuard( aMDataReceived );
            osl::MutexGuard aGuard2( *pMPostUserEvent );
            mlPutDataReceived.Call(this);
        }
    }
    TimeValue sNochEins = {0, 1000000};
    while ( schedule() && bIsInsideCallback )
        wait( sNochEins );

    StartCallback();
    {
        osl::MutexGuard aGuard( aMConnectionClosed );
        osl::MutexGuard aGuard2( *pMPostUserEvent );
        nConnectionClosedEventId = GetpApp()->PostUserEvent( LINK( this, CommunicationLinkViaSocket, ConnectionClosed ) );
    }
}

sal_Bool CommunicationLinkViaSocket::DoTransferDataStream( SvStream *pDataStream, CMProtocol nProtocol )
{
    if ( !isRunning() )
        return sal_False;

    return SimpleCommunicationLinkViaSocket::DoTransferDataStream( pDataStream, nProtocol );
}

/// This is a virtual link!
long CommunicationLinkViaSocket::ConnectionClosed( void* EMPTYARG )
{
    {
        osl::MutexGuard aGuard( aMConnectionClosed );
        nConnectionClosedEventId = 0;   // Attention!! everything else must be done above.
    }
    ShutdownCommunication();
    return CommunicationLink::ConnectionClosed( );
}

/// This is a virtual link!
long CommunicationLinkViaSocket::DataReceived( void* EMPTYARG )
{
    {
        osl::MutexGuard aGuard( aMDataReceived );
        nDataReceivedEventId = 0;   // Attention!! everything else must be done above.
    }
    return CommunicationLink::DataReceived( );
}

IMPL_LINK( CommunicationLinkViaSocket, PutDataReceivedHdl, CommunicationLinkViaSocket*, EMPTYARG )
{
    nDataReceivedEventId = GetpApp()->PostUserEvent( LINK( this, CommunicationLink, DataReceived ) );
    return 0;
}



MultiCommunicationManager::MultiCommunicationManager( sal_Bool bUseMultiChannel )
: CommunicationManager( bUseMultiChannel )
, bGracefullShutdown( sal_True )
{
    ActiveLinks = new CommunicationLinkList;
    InactiveLinks = new CommunicationLinkList;
}

MultiCommunicationManager::~MultiCommunicationManager()
{
    StopCommunication();

    if ( bGracefullShutdown )   // first try to collect all callbacks for closing channels
    {
        Timer aTimeout;
        aTimeout.SetTimeout( 40000 );
        aTimeout.Start();
        sal_uInt16 nLinkCount = 0;
        sal_uInt16 nNewLinkCount = 0;
        while ( aTimeout.IsActive() )
        {
            GetpApp()->Yield();
            nNewLinkCount = GetCommunicationLinkCount();
            if ( nNewLinkCount == 0 )
                aTimeout.Stop();
            if ( nNewLinkCount != nLinkCount )
            {
                aTimeout.Start();
                nLinkCount = nNewLinkCount;
            }
        }
    }


    sal_uInt16 i = ActiveLinks->Count();
    while ( i-- )
    {
        CommunicationLinkRef rTempLink = ActiveLinks->GetObject( i );
        ActiveLinks->Remove( i );
        rTempLink->InvalidateManager();
        rTempLink->ReleaseReference();
    }
    delete ActiveLinks;


    i = InactiveLinks->Count();
    while ( i-- )
    {
        CommunicationLinkRef rTempLink = InactiveLinks->GetObject( i );
        InactiveLinks->Remove( i );
        rTempLink->InvalidateManager();
    }
    delete InactiveLinks;
}

sal_Bool MultiCommunicationManager::StopCommunication()
{

    sal_uInt16 i = ActiveLinks->Count();
    int nFail = 0;
    while ( i )
    {
        if ( !ActiveLinks->GetObject(i-1)->StopCommunication() )
            nFail++;
        i--;
    }

    return nFail == 0;
}

sal_Bool MultiCommunicationManager::IsLinkValid( CommunicationLink* pCL )
{
    if ( ActiveLinks->Seek_Entry( pCL ) )
        return sal_True;
    else
        return sal_False;
}

sal_uInt16 MultiCommunicationManager::GetCommunicationLinkCount()
{
    return ActiveLinks->Count();
}

CommunicationLinkRef MultiCommunicationManager::GetCommunicationLink( sal_uInt16 nNr )
{
    return ActiveLinks->GetObject( nNr );
}

void MultiCommunicationManager::CallConnectionOpened( CommunicationLink* pCL )
{
    CommunicationLinkRef rHold(pCL);
    ActiveLinks->C40_PTR_INSERT(CommunicationLink, pCL);
    rHold->AddRef();

    CommunicationManager::CallConnectionOpened( pCL );
}

void MultiCommunicationManager::CallConnectionClosed( CommunicationLink* pCL )
{
    CommunicationLinkRef rHold(pCL);

    CommunicationManager::CallConnectionClosed( pCL );

    sal_uInt16 nPos;
    if ( ActiveLinks->Seek_Entry( pCL, &nPos ) )
    {
        InactiveLinks->C40_PTR_INSERT(CommunicationLink, pCL);  // without reference
        ActiveLinks->Remove( nPos );
    }
    pCL->ReleaseReference();

    bIsCommunicationRunning = ActiveLinks->Count() > 0;
//  delete pCL;
#if OSL_DEBUG_LEVEL > 1
        rHold->bFlag = sal_True;
#endif
}

void MultiCommunicationManager::DestroyingLink( CommunicationLink *pCL )
{
    sal_uInt16 nPos;
    if ( InactiveLinks->Seek_Entry( pCL, &nPos ) )
        InactiveLinks->Remove( nPos );
    pCL->InvalidateManager();
}



CommunicationManagerClient::CommunicationManagerClient( sal_Bool bUseMultiChannel )
: MultiCommunicationManager( bUseMultiChannel )
{
    ByteString aApplication("Something inside ");
    aApplication.Append( ByteString( DirEntry( Application::GetAppFileName() ).GetName(), gsl_getSystemTextEncoding() ) );
    SetApplication( aApplication );
}



CommunicationManagerServerViaSocket::CommunicationManagerServerViaSocket( sal_uLong nPort, sal_uInt16 nMaxCon, sal_Bool bUseMultiChannel )
: CommunicationManagerServer( bUseMultiChannel )
, nPortToListen( nPort )
, nMaxConnections( nMaxCon )
, pAcceptThread( NULL )
{
}

CommunicationManagerServerViaSocket::~CommunicationManagerServerViaSocket()
{
    StopCommunication();
}

sal_Bool CommunicationManagerServerViaSocket::StartCommunication()
{
    if ( !pAcceptThread )
        pAcceptThread = new CommunicationManagerServerAcceptThread( this, nPortToListen, nMaxConnections );
    return sal_True;
}


sal_Bool CommunicationManagerServerViaSocket::StopCommunication()
{
    delete pAcceptThread;
    pAcceptThread = NULL;

    return CommunicationManagerServer::StopCommunication();
}


void CommunicationManagerServerViaSocket::AddConnection( CommunicationLink *pNewConnection )
{
    CallConnectionOpened( pNewConnection );
}


CommunicationManagerServerAcceptThread::CommunicationManagerServerAcceptThread( CommunicationManagerServerViaSocket* pServer, sal_uLong nPort, sal_uInt16 nMaxCon )
: pMyServer( pServer )
, pAcceptorSocket( NULL )
, nPortToListen( nPort )
, nMaxConnections( nMaxCon )
, nAddConnectionEventId( 0 )
, xmNewConnection( NULL )
{
    if ( !pMPostUserEvent )
        pMPostUserEvent = new osl::Mutex;
    create();
}


CommunicationManagerServerAcceptThread::~CommunicationManagerServerAcceptThread()
{
#ifndef aUNX        // because the accept can't be canceled - this way the prog's terminated at leastW
    // #62855# pl: counts for other unix systems too
    // correct solution would be of course to write something on the pipe which
    // the thread recognizes as a cancelling condition
    // or at least kill instead of join
    terminate();
    if ( pAcceptorSocket )
        pAcceptorSocket->close();

    join();

    if ( pAcceptorSocket )
    {
        delete pAcceptorSocket;
        pAcceptorSocket = NULL;
    }
#else
    DEBUGPRINTF ("Destructor CommunicationManagerServerAcceptThread �bersprungen!!!! (wegen Solaris BUG)\n");
#endif
    {
        osl::MutexGuard aGuard( aMAddConnection );
        if ( nAddConnectionEventId )
        {
            GetpApp()->RemoveUserEvent( nAddConnectionEventId );
            nAddConnectionEventId = 0;
            CommunicationLinkRef xNewConnection = GetNewConnection();
            INFO_MSG( CByteString("Event gel�scht"),
                CByteString( "AddConnectionEvent aus Queue gel�scht"),
                CM_MISC, xNewConnection );
            xNewConnection->InvalidateManager();
            xNewConnection.Clear();
        }
    }
}

void CommunicationManagerServerAcceptThread::run()
{
    if ( !nPortToListen )
        return;

    pAcceptorSocket = new osl::AcceptorSocket();
    osl::SocketAddr Addr;
    Addr.setPort( nPortToListen );
    pAcceptorSocket->setOption( osl_Socket_OptionReuseAddr, 1 );
     if ( !pAcceptorSocket->bind( Addr ) )
    {
        return;
    }
    if ( !pAcceptorSocket->listen( nMaxConnections ) )
    {
        return;
    }


    osl::StreamSocket* pStreamSocket = NULL;

    while ( schedule() )
    {
        pStreamSocket = new osl::StreamSocket;
        switch ( pAcceptorSocket->acceptConnection( *pStreamSocket ) )
        {
        case osl_Socket_Ok:
            {
                pStreamSocket->setOption( osl_Socket_OptionTcpNoDelay, 1 );

                TimeValue sNochEins = {0, 100};
                while ( schedule() && xmNewConnection.Is() )
                    wait( sNochEins );
                xmNewConnection = new CommunicationLinkViaSocket( pMyServer, pStreamSocket );
                xmNewConnection->StartCallback();
                {
                    osl::MutexGuard aGuard( aMAddConnection );
                    osl::MutexGuard aGuard2( *pMPostUserEvent );
                    nAddConnectionEventId = GetpApp()->PostUserEvent( LINK( this, CommunicationManagerServerAcceptThread, AddConnection ) );
                }
            }
            break;
        case osl_Socket_TimedOut:
            delete pStreamSocket;
            pStreamSocket = NULL;
            break;
        case osl_Socket_Error:
            delete pStreamSocket;
            pStreamSocket = NULL;
            break;

        case osl_Socket_Interrupted:
        case osl_Socket_InProgress:
        default:
            break;
        }
    }
}


IMPL_LINK( CommunicationManagerServerAcceptThread, AddConnection, void*, EMPTYARG )
{
    {
        osl::MutexGuard aGuard( aMAddConnection );
        nAddConnectionEventId = 0;
    }
    pMyServer->AddConnection( xmNewConnection );
    xmNewConnection.Clear();
    return 1;
}

CommunicationManagerClientViaSocket::CommunicationManagerClientViaSocket( sal_Bool bUseMultiChannel )
: CommunicationManagerClient( bUseMultiChannel )
, aHostToTalk( "" )
, nPortToTalk( 0 )
{
}

CommunicationManagerClientViaSocket::~CommunicationManagerClientViaSocket()
{
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
