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
#ifndef _SIMPLECM_HXX
#define _SIMPLECM_HXX

#include <tools/link.hxx>
#include <tools/string.hxx>
#include <tools/stream.hxx>
#include <osl/socket.hxx>
#include <tools/debug.hxx>
#include <tools/datetime.hxx>

#include <automation/commdefines.hxx>

// CM stands for CommunicationManager
#define CM_UNLIMITED_CONNECTIONS    0xffff

typedef sal_uInt16 CM_NameType;
#define CM_DOTTED   ( (CM_NameType) 01 )
#define CM_FQDN     ( (CM_NameType) 02 )

typedef sal_uInt16 CM_InfoType;
// only one of these three defines may be used
#define CM_NO_TEXT      ( (CM_InfoType) 01 )
#define CM_SHORT_TEXT   ( (CM_InfoType) 02 )
#define CM_VERBOSE_TEXT ( (CM_InfoType) 03 )

#define CM_OPEN         ( (CM_InfoType) 0x0004 )
#define CM_CLOSE        ( (CM_InfoType) 0x0008 )
#define CM_RECEIVE      ( (CM_InfoType) 0x0010 )
#define CM_SEND         ( (CM_InfoType) 0x0020 )
#define CM_ERROR        ( (CM_InfoType) 0x0040 )
#define CM_MISC         ( (CM_InfoType) 0x0080 )

#define CM_USER_1       ( (CM_InfoType) 0x0100 )
#define CM_USER_2       ( (CM_InfoType) 0x0200 )
#define CM_USER_3       ( (CM_InfoType) 0x0400 )
#define CM_USER_4       ( (CM_InfoType) 0x0800 )

#define CM_ALL          ( CM_OPEN | CM_CLOSE | CM_RECEIVE | CM_SEND | CM_ERROR | CM_MISC )
#define CM_NONE         ( 0 )

#define CByteString( constAsciiStr ) ByteString( RTL_CONSTASCII_STRINGPARAM ( constAsciiStr ) )

#define INFO_MSG( Short, Long, Type, CLink ) \
{ \
    if ( (Type & GetInfoType()) > 0 ) \
    { \
        switch ( GetInfoType() & 03 ) \
        { \
            case CM_NO_TEXT: \
                { \
                       ByteString aByteString; \
                    CallInfoMsg( InfoString( aByteString, Type, CLink ) ); \
                } \
                break; \
            case CM_SHORT_TEXT: \
                { \
                       ByteString aByteString( Short ); \
                    CallInfoMsg( InfoString( aByteString, Type, CLink ) ); \
                } \
                break; \
            case CM_VERBOSE_TEXT: \
                { \
                       ByteString aByteString( Long ); \
                    CallInfoMsg( InfoString( aByteString, Type, CLink ) ); \
                } \
                break; \
            default: \
                break; \
        } \
    } \
}\

class CommunicationLink;

SV_DECL_REF( CommunicationLink )

class InfoString : public ByteString
{
public:
    InfoString( ByteString &nMsg, CM_InfoType nIT, CommunicationLink *pCL = NULL ): ByteString( nMsg ), nInfoType( nIT ), pCommLink( pCL ) {;}
    CM_InfoType GetInfoType(){ return nInfoType; }
    CommunicationLinkRef GetCommunicationLink(){ return pCommLink; }
private:
    CM_InfoType nInfoType;
    CommunicationLinkRef pCommLink;
};

class PacketHandler;
class CommunicationManager;
class MultiCommunicationManager;
class CommunicationManagerServerAcceptThread;
class CommunicationLink : public SvRefBase
{
protected:
    friend class CommunicationManager;
    friend class MultiCommunicationManager;
    friend class CommunicationManagerServerAcceptThread;
    // may not be stopped between the stream's reception and the callback's end

protected:  // so that it can only be deleted via Ref
    virtual ~CommunicationLink();
    void InvalidateManager() { pMyManager = NULL; }

    PacketHandler* pPacketHandler;

public:
    CommunicationLink( CommunicationManager *pMan );

    virtual sal_Bool StopCommunication()=0;
    virtual sal_Bool IsCommunicationError()=0;
    CommunicationManager* GetCommunicationManager(){ return pMyManager; }

    virtual ByteString GetCommunicationPartner( CM_NameType eType )=0;

    virtual ByteString GetMyName( CM_NameType eType )=0;

    virtual SvStream* GetBestCommunicationStream()=0;

    /** will call virtual function DoTransferDataStream to do actual work
        Purpos is to allow housekeeping
    **/
    sal_Bool TransferDataStream( SvStream *pDataStream, CMProtocol nProtocol = CM_PROTOCOL_OLDSTYLE );

    // delivers the ID that is named by the server
    // Due to that virtual communications over a physical link can be realised.
    // Because the communication must remain compatible to older versions the receiver must guess whether
    // the new or the old one is used because the order's head changes then
    sal_uInt16 GetProtocol(){ return nServiceProtocol; }

    // Commits the stream. The caller is responsible for the deletion.
    // The method must be called because there would be no further data received otherwise.
    SvStream* GetServiceData(){ SvStream *pTemp = pServiceData; pServiceData = NULL; return pTemp; }

    /// Allows for the release of the next callbacks. Is also called implicitly.
    void FinishCallback(){ bIsInsideCallback = sal_False; }

    /// Synchronous reception of the data. Only for command line - empty implementation for other uses.
    virtual sal_Bool ReceiveDataStream(){ return sal_False; }

    /// Statistics
    DateTime GetStart() { return aStart; }
    sal_uLong GetTotalBytes() { return nTotalBytes; }
    DateTime GetLastAccess() { return aLastAccess; }
    const ByteString& GetApplication() { return maApplication; }
    virtual void SetApplication( const ByteString& aApp );

protected:
    void CallInfoMsg( InfoString aMsg );
    CM_InfoType GetInfoType();
    CommunicationManager *pMyManager;
// These methods are called in the main context and are handed over to the manager.
    virtual DECL_LINK( ConnectionClosed, void* = NULL );
    virtual DECL_LINK( DataReceived, void* = NULL );

    virtual sal_Bool DoTransferDataStream( SvStream *pDataStream, CMProtocol nProtocol = CM_PROTOCOL_OLDSTYLE );

    SvStream *pServiceData;
    sal_uInt16 nServiceProtocol;
    sal_uInt16 nServiceHeaderType;

    /// Prevents the early release of the next callback.
    void StartCallback(){ bIsInsideCallback = sal_True; }
    sal_Bool bIsInsideCallback;

    virtual sal_Bool SendHandshake( HandshakeType aHandshakeType, SvStream* pData = NULL)=0;

    virtual sal_Bool ShutdownCommunication() = 0;   /// Really stop the Communication

    /// Statistics
    DateTime aStart;
    sal_uLong nTotalBytes;
    DateTime aLastAccess;

private:
    ByteString maApplication;

#if OSL_DEBUG_LEVEL > 1
public:
    // misc (debuging) purposes
    sal_Bool bFlag;
    sal_uLong nSomething;
#endif

};

SV_IMPL_REF( CommunicationLink );

class CommonSocketFunctions;
class CommunicationManager
{
    friend class CommunicationLink;
    friend class CommonSocketFunctions;
public:
    CommunicationManager( sal_Bool bUseMultiChannel = sal_False );
    virtual ~CommunicationManager();

    virtual sal_Bool StartCommunication()=0;
    virtual sal_Bool StartCommunication( String aApp, String aParams );
    virtual sal_Bool StartCommunication( ByteString aHost, sal_uLong nPort );
    virtual sal_Bool StopCommunication()=0;
    virtual sal_Bool IsCommunicationRunning() { return bIsCommunicationRunning; }
//  virtual sal_Bool IsCommunicationError();


    virtual ByteString GetMyName( CM_NameType eType );

    virtual sal_Bool IsLinkValid( CommunicationLink* pCL )=0;   // necessary for call in destructor

    virtual sal_uInt16 GetCommunicationLinkCount()=0;
    virtual CommunicationLinkRef GetCommunicationLink( sal_uInt16 nNr )=0;

    CommunicationLinkRef GetLastNewLink() { return xLastNewLink; }

    void SetConnectionOpenedHdl( Link lConnectionOpened ){ mlConnectionOpened = lConnectionOpened; }
    void SetConnectionClosedHdl( Link lConnectionClosed ){ mlConnectionClosed = lConnectionClosed; }
    void SetDataReceivedHdl( Link lDataReceived ){ mlDataReceived = lDataReceived; }
    void SetInfoMsgHdl( Link lInfoMsg ){ mlInfoMsg = lInfoMsg; }

    void SetInfoType( CM_InfoType nIT ){ nInfoType = nIT; }
    CM_InfoType GetInfoType(){ return nInfoType; }

    sal_Bool IsMultiChannel(){ return bIsMultiChannel; }
    void SetApplication( const ByteString& aApp, sal_Bool bRunningLinks = sal_False );
    const ByteString& GetApplication() { return maApplication; }

protected:
    // These methods are called inside. They care for housekeeping if applicable
    // and call the respective method then.
    virtual void CallConnectionOpened( CommunicationLink* pCL );
    virtual void CallConnectionClosed( CommunicationLink* pCL );
    void CallDataReceived( CommunicationLink* pCL );
    void CallInfoMsg( InfoString aMsg );

    CM_InfoType nInfoType;

    //  These routines call the link or are overloaded.
    virtual void ConnectionOpened( CommunicationLink* pCL ){ mlConnectionOpened.Call( pCL ); }
    virtual void ConnectionClosed( CommunicationLink* pCL ){ mlConnectionClosed.Call( pCL ); }
    virtual void DataReceived( CommunicationLink* pCL ){ mlDataReceived.Call( pCL ); }
    virtual void InfoMsg( InfoString aMsg ){ mlInfoMsg.Call( &aMsg ); }

    sal_Bool bIsCommunicationRunning;

    virtual void DestroyingLink( CommunicationLink *pCL )=0;

private:
    ByteString maApplication;
    Link mlConnectionOpened;
    Link mlConnectionClosed;
    Link mlDataReceived;
    Link mlInfoMsg;
    CommunicationLinkRef xLastNewLink;

    sal_Bool bIsMultiChannel;
};

class ICommunicationManagerClient
{
    friend class CommonSocketFunctions;
protected:
    virtual sal_Bool RetryConnect() { return sal_False; }   // might be able to start the application
};

class TCPIO;
class SimpleCommunicationLinkViaSocket : public CommunicationLink
{
public:
    virtual sal_Bool IsCommunicationError();
    virtual sal_Bool StopCommunication();

    virtual ByteString GetCommunicationPartner( CM_NameType eType );
    virtual ByteString GetMyName( CM_NameType eType );
    virtual SvStream* GetBestCommunicationStream();
    virtual void SetApplication( const ByteString& aApp );

private:
    ByteString aCommunicationPartner;
    ByteString aMyName;

    TCPIO* pTCPIO;
    osl::StreamSocket* pStreamSocket;

protected:
    SimpleCommunicationLinkViaSocket( CommunicationManager *pMan, osl::StreamSocket* pSocket );
    virtual ~SimpleCommunicationLinkViaSocket();

    osl::StreamSocket* GetStreamSocket() { return pStreamSocket; }
    void SetStreamSocket( osl::StreamSocket* pSocket );

    SvStream *pReceiveStream;
    sal_Bool DoReceiveDataStream();             /// Recieve DataPacket from Socket
    virtual sal_Bool SendHandshake( HandshakeType aHandshakeType, SvStream* pData = NULL);
    sal_Bool bIsRequestShutdownPending;
    virtual void WaitForShutdown()=0;
    void SetNewPacketAsCurrent();
};

class CommonSocketFunctions
{
public:
    sal_Bool DoStartCommunication( CommunicationManager *pCM, ICommunicationManagerClient *pCMC, ByteString aHost, sal_uLong nPort );
protected:
    virtual CommunicationLink *CreateCommunicationLink( CommunicationManager *pCM, osl::ConnectorSocket* pCS )=0;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
