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


/*************************************************************************
 *
 *    ATTENTION
 *    This file is intended to work inside and outside the StarOffice environment.
 *    Only adaption of file commtypes.hxx should be necessary. Else it is a bug!
 *
 ************************************************************************/

#include "packethandler.hxx"
#include <automation/commtypes.hxx>
#include <automation/commdefines.hxx>
#include "communiio.hxx"
#include <osl/diagnose.h>

/**
Forces switch to multichannel headers even for old communication Method
**/
#define FORCE_MULTI_CHANNEL_HEADERS


PacketHandler::PacketHandler( ITransmiter* pTransmitter_, IReceiver* pReceiver_, comm_BOOL bMC )
: pTransmitter( pTransmitter_ )
, pReceiver( pReceiver_ )
, bMultiChannel( bMC )
{
}

unsigned char PacketHandler::CalcCheckByte( comm_UINT32 nBytes )
{
    comm_UINT16 nRes = 0;
    nRes += HIBYTE( HIWORD( nBytes ) ) ^ 0xf0;
    nRes += LOBYTE( HIWORD( nBytes ) ) ^ 0x0f;
    nRes += HIBYTE( LOWORD( nBytes ) ) ^ 0xf0;
    nRes += LOBYTE( LOWORD( nBytes ) ) ^ 0x0f;

    nRes ^= HIBYTE( nRes );

    return LOBYTE( nRes );
}


#define READ_SOCKET( pBuffer, nLength )\
    if ( !bWasError )\
    {\
        bWasError |= pReceiver->ReceiveBytes( pBuffer, nLength ) != C_ERROR_NONE;\
    }

#define READ_SOCKET_LEN( pBuffer, nLength, nTotal )\
    READ_SOCKET( pBuffer, nLength );\
    if ( !bWasError )\
        {nTotal += nLength;}

comm_BOOL PacketHandler::ReceiveData( void* &pData, comm_UINT32 &nLen )
{
    DBG_ASSERT( !pData, "pData should be NULL -> memory leak" );

    nLen = 0;
    pData = NULL;
    comm_BOOL bWasError = sal_False;
    comm_BOOL bForceMultiChannelThisPacket = sal_False;
    if ( pReceiver )
    {
        comm_UINT32 nBytes = 0;
        nReceiveProtocol = CM_PROTOCOL_OLDSTYLE;
        nReceiveHeaderType = CH_NoHeader;

        READ_SOCKET( &nBytes, sizeof(nBytes) )
        if ( bWasError )
            return sal_False;

        if ( 0xFFFFFFFF == nBytes )     // explicit request for this data package to switch to MultiChannel
        {
            READ_SOCKET( &nBytes, sizeof(nBytes) )
            if ( bWasError )
                return sal_False;
            bForceMultiChannelThisPacket = sal_True;
        }

        nBytes = NETDWORD( nBytes );

        if ( bMultiChannel || bForceMultiChannelThisPacket )
        {
            comm_UINT32 nReadSoFar = 0;
            comm_UINT32 nHeaderReadSoFar = 0;

            unsigned char nLenCheck = 0;
            READ_SOCKET_LEN( &nLenCheck, 1, nReadSoFar );
            bWasError |= nLenCheck != CalcCheckByte( nBytes );


            comm_UINT16 nHeaderBytes;
            READ_SOCKET_LEN( &nHeaderBytes, 2, nReadSoFar );
            nHeaderBytes = NETWORD( nHeaderBytes );
            bWasError |= !(nBytes >= nReadSoFar + nHeaderBytes);

            READ_SOCKET_LEN( &nReceiveHeaderType, 2, nHeaderReadSoFar );
            nReceiveHeaderType = NETWORD( nReceiveHeaderType );

            switch ( nReceiveHeaderType )
            {
            case CH_SimpleMultiChannel:
                {
                    READ_SOCKET_LEN( &nReceiveProtocol, 2, nHeaderReadSoFar );
                    nReceiveProtocol = NETWORD( nReceiveProtocol );
                }
                break;
            case CH_Handshake:
                {
                }
                break;
            default:
                {
                    OSL_FAIL("Unbekannter Headertyp in der Kommunikation");
                    bWasError = sal_True;
                }

            }

            if ( bWasError )
                return sal_False;


            while ( nHeaderBytes > nHeaderReadSoFar )
            {
                unsigned char nDummy;
                READ_SOCKET_LEN( &nDummy, 1, nHeaderReadSoFar );
            }

            nReadSoFar += nHeaderReadSoFar;
            nBytes -= nReadSoFar;

        }

        /* @@@ Notes @@@
         *
         * 1) a 'void*' allocated via 'new char[]' is always deallocated
         * via plain 'delete()', not via array 'delete[]()'; it's just
         * raw memory.
         *
         * 2) as the caller of this routine later-on changes ownership
         * of 'pData' via 'SvMemoryStream::SetBuffer()' (in 'simplecm.cxx',
         * 'SimpleCommunicationLinkViaSocket::DoReceiveDataStream()'),
         * the allocator used here for 'void* pData' must match the
         * deallocator used in 'SvMemoryStream::FreeMemory()', i.e.
         * '::operator delete()'.
         */
        pData = ::operator new(nBytes);
        READ_SOCKET( pData, nBytes )
        if ( bWasError )
        {
            ::operator delete(pData), pData = 0;
            return sal_False;
        }
        nLen = nBytes;
    }
    else
        bWasError = sal_True;

    return !bWasError;
}

/*#define WRITE_SOCKET( pBuffer, nLength )\
    if ( !bWasError )\
        bWasError |= !pStreamSocket || (pStreamSocket->write( pBuffer, nLength ) != nLength)*/

#define WRITE_SOCKET( pBuffer, nLength )\
    if ( !bWasError )\
        {bWasError |= pTransmitter->TransferBytes( pBuffer, nLength ) != C_ERROR_NONE;}



comm_BOOL PacketHandler::TransferData( const void* pData, comm_UINT32 nLen, CMProtocol nProtocol )
{
    comm_UINT32 nBuffer = nLen;
    comm_BOOL bWasError = sal_False;

#ifndef FORCE_MULTI_CHANNEL_HEADERS
    if ( bMultiChannel )
#endif
        nBuffer += 1+2+2+2; // for a CH_SimpleMultiChannel

#ifdef FORCE_MULTI_CHANNEL_HEADERS
    if ( !bMultiChannel )
    {
        comm_UINT32 n32;
        n32 = 0xffffffff;   // switch to MultiChannel
        n32 = NETDWORD( n32 );
        WRITE_SOCKET( &n32, 4 );
    }
#endif


    comm_UINT32 nNetworkBuffer = NETDWORD( nBuffer );
    WRITE_SOCKET( &nNetworkBuffer, sizeof(nNetworkBuffer) );


#ifndef FORCE_MULTI_CHANNEL_HEADERS
    if ( bMultiChannel )
#endif
    {
        comm_UINT16 n16;
        unsigned char c;

        c = CalcCheckByte( nBuffer );
        WRITE_SOCKET( &c, 1 );

        n16 = 4;    // header length for a CH_SimpleMultiChannel
        n16 = NETWORD( n16 );
        WRITE_SOCKET( &n16, 2 );

        n16 = CH_SimpleMultiChannel;    // header type
        n16 = NETWORD( n16 );
        WRITE_SOCKET( &n16, 2 );

        nProtocol = NETWORD( nProtocol );
        WRITE_SOCKET( &nProtocol, 2 );
    }

    WRITE_SOCKET( pData, nLen );
    return !bWasError;
}

comm_BOOL PacketHandler::SendHandshake( HandshakeType aHandshakeType, const void* pData, comm_UINT32 nLen )
{
    comm_BOOL bWasError = sal_False;

    comm_UINT32 nBuffer = 0;

//  if ( pMyManager->IsMultiChannel() )     we always send FFFFFFFF before -> always MultiChannel (or GPF for older ones)
        nBuffer += 1+2+2;   // for a CH_Handshake

    nBuffer += 2;   // for the Handshake's type

    switch ( aHandshakeType )
    {
        case CH_REQUEST_HandshakeAlive:
            nBuffer += 0;   // no extra data
            break;
        case CH_RESPONSE_HandshakeAlive:
            nBuffer += 0;   // no extra data
            break;
        case CH_REQUEST_ShutdownLink:
            nBuffer += 0;   // no extra data
            break;
        case CH_ShutdownLink:
            nBuffer += 0;   // no extra data
            break;
        case CH_SUPPORT_OPTIONS:
            nBuffer += 2 ;  // one word extradata for options
            break;
        case CH_SetApplication:
            nBuffer += 0 ;  // one word extradata for options
            break;
        default:
            OSL_FAIL("Unknown HandshakeType");
    }

    if ( pData )
        nBuffer += nLen;    // Extra data in Buffer

    comm_UINT32 n32;
    n32 = 0xffffffff;   // switch to MultiChannel
    n32 = NETDWORD( n32 );
    WRITE_SOCKET( &n32, 4 );

    comm_UINT32 nNetworkBuffer = NETDWORD( nBuffer );
    WRITE_SOCKET( &nNetworkBuffer, sizeof(nNetworkBuffer) );


    comm_UINT16 n16;
    unsigned char c;

    c = CalcCheckByte( nBuffer );
    WRITE_SOCKET( &c, 1 );

    n16 = 2;    // header's length for a CH_Handshake
    n16 = NETWORD( n16 );
    WRITE_SOCKET( &n16, 2 );

    n16 = CH_Handshake; // the header's type
    n16 = NETWORD( n16 );
    WRITE_SOCKET( &n16, 2 );

    n16 = aHandshakeType;   // the header's type
    n16 = NETWORD( n16 );
    WRITE_SOCKET( &n16, 2 );


    switch ( aHandshakeType )
    {
        case CH_SUPPORT_OPTIONS:
            n16 = OPT_USE_SHUTDOWN_PROTOCOL;
            n16 = NETWORD( n16 );
            WRITE_SOCKET( &n16, 2 );
            break;
    }

    if ( pData )
        WRITE_SOCKET( pData, nLen );

    return !bWasError;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
