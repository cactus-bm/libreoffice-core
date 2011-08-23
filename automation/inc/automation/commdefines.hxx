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

#include <automation/commtypes.hxx>
#include <osl/endian.h>

#ifndef MAKEDWORD
#define MAKEDWORD(wl, wh)   ((sal_uInt32)((wl) & 0xFFFF) | (((sal_uInt32)(wh) & 0xFFFF) << 16))
#endif
#ifndef LOWORD
#define LOWORD(d)           ((sal_uInt16)((sal_uInt32)(d) & 0xFFFF))
#endif
#ifndef HIWORD
#define HIWORD(d)           ((sal_uInt16)(((sal_uInt32)(d) >> 16) & 0xFFFF))
#endif
#ifndef MAKEWORD
#define MAKEWORD(bl, bh)    ((sal_uInt16)((bl) & 0xFF) | (((sal_uInt16)(bh) & 0xFF) << 8))
#endif
#ifndef LOBYTE
#define LOBYTE(w)           ((comm_BYTE)((sal_uInt16)(w) & 0xFF))
#endif
#ifndef HIBYTE
#define HIBYTE(w)           ((comm_BYTE)(((sal_uInt16)(w) >> 8) & 0xFF))
#endif
#ifndef MAKEBYTE
#define MAKEBYTE(nl, nh)    ((comm_BYTE)(((nl) & 0x0F) | (((nh) & 0x0F) << 4)))
#endif
#ifndef LONIBBLE
#define LONIBBLE(b)         ((comm_BYTE)((b) & 0x0F))
#endif
#ifndef HINIBBLE
#define HINIBBLE(b)         ((comm_BYTE)(((b) >> 4) & 0x0F))
#endif

#ifndef SWAPWORD
#define SWAPWORD(w)         MAKEWORD(HIBYTE(w),LOBYTE(w))
#endif
#ifndef SWAPDWORD
#define SWAPDWORD(d)        MAKEDWORD(SWAPWORD(HIWORD(d)),SWAPWORD(LOWORD(d)))
#endif

#ifdef OSL_BIGENDIAN
#ifndef NETWORD
#define NETWORD(w)          (sal_uInt16)(w)
#endif
#ifndef NETDWORD
#define NETDWORD(d)         (sal_uInt32)(d)
#endif
#endif // OSL_BIGENDIAN

#ifdef OSL_LITENDIAN
#ifndef NETWORD
#define NETWORD(w)          MAKEWORD(HIBYTE(w),LOBYTE(w))
#endif
#ifndef NETDWORD
#define NETDWORD(d)         MAKEDWORD(NETWORD(HIWORD(d)),NETWORD(LOWORD(d)))
#endif
#endif // OSL_LITENDIAN

/**
    There are two types of data packages:
    The first one contains in the first 4 bytes the length information and in the following the data.
    The length information refers to the data only, without the length information itself.

    The second type of data packages contains headers with further information as described below.

    It's switched with the Boolean bUseMultiChanell in the manager's constructor.
**/
/**
Defines for header types:

general header:
        bytes  length       content
        0..3        4       the package's length without these 4 bytes
        4           1       Checksum concerning the length. If it's incorrect, the connection is closed.
        5..6        2       length of the header without these 2 bytes
        7..8        2       type of the header

CH_SimpleMultiChannel:
        9..10       2       channel
CH_Handshake                Internal Use ONLY
                            No further data!

**/
typedef comm_UINT16 CMProtocol;

#define CM_PROTOCOL_OLDSTYLE        (CMProtocol)0x0001
#define CM_PROTOCOL_MARS            (CMProtocol)0x0001
#define CM_PROTOCOL_BROADCASTER     (CMProtocol)0x0002
#define CM_PROTOCOL_USER_START      (CMProtocol)0x0100

typedef comm_UINT16 HandshakeType;
typedef comm_UINT16 CommunicationOption;

#define CH_NoHeader                 0x0000
#define CH_SimpleMultiChannel       0x0001
#define CH_Handshake                0x0002

#define CH_REQUEST_HandshakeAlive   ((HandshakeType)0x0101) /// aks for alive reply
#define CH_RESPONSE_HandshakeAlive  ((HandshakeType)0x0102) /// alive reply

/**
    Announce supported options:
    Client announces available options
    Server returns subset of these options (note that the sbset can be the entire set also)
**/
#define CH_SUPPORT_OPTIONS          ((HandshakeType)0x0103)
#define OPT_USE_SHUTDOWN_PROTOCOL   ((CommunicationOption)0x0001)

/// these are for making sure all Data is read prior to shutting sown the link
#define CH_REQUEST_ShutdownLink     ((HandshakeType)0x0104) /// Request to Shutdown this link
#define CH_ShutdownLink             ((HandshakeType)0x0105) /// Shutdown this link

#define CH_SetApplication           ((HandshakeType)0x0106) /// Set Description of Client

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
