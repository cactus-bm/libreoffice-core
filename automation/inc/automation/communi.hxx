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

#ifndef _COMMUNI_HXX
#define _COMMUNI_HXX

#include "automation/communidllapi.h"

#include <svl/svarray.hxx>
#include <osl/thread.hxx>
#include <osl/mutex.hxx>
#include <vcl/timer.hxx>
#include <automation/simplecm.hxx>

class SvStream;
class SvMemoryStream;
//class Application;

class CommunicationManagerServerAcceptThread;
SV_DECL_PTRARR_SORT( CommunicationLinkList, CommunicationLink*, 1, 10 )

class COMMUNI_DLLPUBLIC MultiCommunicationManager : public CommunicationManager
{
public:
    MultiCommunicationManager( sal_Bool bUseMultiChannel = sal_False );
    virtual ~MultiCommunicationManager();
    virtual sal_Bool StopCommunication();
    virtual sal_Bool IsLinkValid( CommunicationLink* pCL );
    virtual sal_uInt16 GetCommunicationLinkCount();
    virtual CommunicationLinkRef GetCommunicationLink( sal_uInt16 nNr );

    void DoQuickShutdown( sal_Bool bQuickShutdown = sal_True) { bGracefullShutdown = !bQuickShutdown; }

protected:
    virtual void CallConnectionOpened( CommunicationLink* pCL );
    virtual void CallConnectionClosed( CommunicationLink* pCL );
    CommunicationLinkList *ActiveLinks;
    CommunicationLinkList *InactiveLinks;       /// CommunicationLinks that have not yet logged off themselves but already have received
                                                /// a StopCommunication or a ConnectionTerminated
    virtual void DestroyingLink( CommunicationLink *pCL );

    sal_Bool bGracefullShutdown;
};

class COMMUNI_DLLPUBLIC CommunicationManagerServer : public MultiCommunicationManager
{
public:
    CommunicationManagerServer( sal_Bool bUseMultiChannel = sal_False ):MultiCommunicationManager( bUseMultiChannel ){;}
};

class COMMUNI_DLLPUBLIC CommunicationManagerClient : public MultiCommunicationManager, public ICommunicationManagerClient
{
public:
    CommunicationManagerClient( sal_Bool bUseMultiChannel = sal_False );
};

class COMMUNI_DLLPUBLIC CommunicationLinkViaSocket : public SimpleCommunicationLinkViaSocket, public osl::Thread
{
public:
    CommunicationLinkViaSocket( CommunicationManager *pMan, osl::StreamSocket* pSocket );
    virtual ~CommunicationLinkViaSocket();

    virtual sal_Bool IsCommunicationError();
    virtual sal_Bool DoTransferDataStream( SvStream *pDataStream, CMProtocol nProtocol = CM_PROTOCOL_OLDSTYLE );

    // These are virtual links!
    virtual long ConnectionClosed( void* = NULL );
    virtual long DataReceived( void* = NULL );

    virtual sal_Bool StopCommunication();

    void SetPutDataReceivedHdl( Link lPutDataReceived ){ mlPutDataReceived = lPutDataReceived; }
    Link GetDataReceivedLink () {Link aLink = LINK( this, CommunicationLinkViaSocket, DataReceived ); return aLink;}
    DECL_LINK( PutDataReceivedHdl, CommunicationLinkViaSocket* );

protected:
    virtual void SAL_CALL run();

    virtual sal_Bool ShutdownCommunication();
    sal_uLong nConnectionClosedEventId;
    sal_uLong nDataReceivedEventId;
    osl::Mutex aMConnectionClosed;  // necessary because no event can be managed before the variable is set
    osl::Mutex aMDataReceived;      // necessary because no event can be managed before the variable is set
    virtual void WaitForShutdown();

    DECL_LINK( ShutdownLink, void* );
       Timer aShutdownTimer;
    sal_Bool bShutdownStarted;
    sal_Bool bDestroying;
    Link mlPutDataReceived;
};

class COMMUNI_DLLPUBLIC CommunicationManagerServerViaSocket : public CommunicationManagerServer
{
    friend class CommunicationManagerServerAcceptThread;
public:
    using CommunicationManager::StartCommunication;

    CommunicationManagerServerViaSocket( sal_uLong nPort, sal_uInt16 nMaxCon, sal_Bool bUseMultiChannel = sal_False );
    virtual ~CommunicationManagerServerViaSocket();

    virtual sal_Bool StartCommunication();
    virtual sal_Bool StopCommunication();

protected:
    sal_uLong nPortToListen;
    sal_uInt16 nMaxConnections;

private:
    CommunicationManagerServerAcceptThread *pAcceptThread;
    void AddConnection( CommunicationLink *pNewConnection );
};

class COMMUNI_DLLPUBLIC CommunicationManagerServerAcceptThread: public osl::Thread
{
public:
    CommunicationManagerServerAcceptThread( CommunicationManagerServerViaSocket* pServer, sal_uLong nPort, sal_uInt16 nMaxCon = CM_UNLIMITED_CONNECTIONS );
    virtual ~CommunicationManagerServerAcceptThread();
    CommunicationLinkRef GetNewConnection(){ CommunicationLinkRef xTemp = xmNewConnection; xmNewConnection.Clear(); return xTemp; }

protected:
    virtual void SAL_CALL run();

private:
    CommunicationManagerServerViaSocket* pMyServer;
    osl::AcceptorSocket* pAcceptorSocket;
    sal_uLong nPortToListen;
    sal_uInt16 nMaxConnections;
    sal_uLong nAddConnectionEventId;
    osl::Mutex aMAddConnection; // necessary because no event can be managed before the variable is set
    void CallInfoMsg( InfoString aMsg ){ pMyServer->CallInfoMsg( aMsg ); }
    CM_InfoType GetInfoType(){ return pMyServer->GetInfoType(); }

    // these are used for the connection's transport from the thread to the mainthread
    CommunicationLinkRef xmNewConnection;
    DECL_LINK( AddConnection, void* );
};

class COMMUNI_DLLPUBLIC CommunicationManagerClientViaSocket : public CommunicationManagerClient, CommonSocketFunctions
{
public:
    using CommunicationManager::StartCommunication;

    CommunicationManagerClientViaSocket( sal_Bool bUseMultiChannel = sal_False );
    virtual ~CommunicationManagerClientViaSocket();

    virtual sal_Bool StartCommunication(){ return StartCommunication( aHostToTalk, nPortToTalk );}
    virtual sal_Bool StartCommunication( ByteString aHost, sal_uLong nPort ){ return DoStartCommunication( this, (ICommunicationManagerClient*) this, aHost, nPort  );}

private:
    ByteString aHostToTalk;
    sal_uLong nPortToTalk;
protected:
    virtual CommunicationLink *CreateCommunicationLink( CommunicationManager *pCM, osl::ConnectorSocket* pCS ){ return new CommunicationLinkViaSocket( pCM, pCS ); }
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
