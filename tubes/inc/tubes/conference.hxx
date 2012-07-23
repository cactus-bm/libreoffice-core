/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * Copyright (C) 2012 Red Hat, Inc., Eike Rathke <erack@redhat.com>
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */

#ifndef INCLUDED_TUBES_CONFERENCE_HXX
#define INCLUDED_TUBES_CONFERENCE_HXX

#include <sal/config.h>
#include "tubes/tubesdllapi.h"
#include <rtl/ustring.hxx>
#include <telepathy-glib/telepathy-glib.h>
#include <queue>
#include <tubes/warnings_guard_boost_signals2.hpp>

class TeleManager;

/** Conference setup by TeleManager */
class TeleConference
{
public:

    TeleConference( TeleManager* pManager, TpAccount *pAccount, TpDBusTubeChannel* pChannel, const OString sUuid = OString() );
    ~TeleConference();

    /// Close channel and call finalize()
    TUBES_DLLPUBLIC void    close();

    /// Unrefs, unregisters from manager and calls dtor if last reference!
    void                    finalize();

    TeleManager*            getManager() const  { return mpManager; }

    TUBES_DLLPUBLIC bool    sendPacket( const OString& rPacket );

    /** Pop a received packet. */
    bool                    popPacket( OString& rPacket );

    /** Queue incoming data as OString */
    void                    queue( const char* pPacket, int nSize );
    void                    queue( const OString& rPacket );

    TUBES_DLLPUBLIC void    invite( TpContact *pContact );

    /** Emitted when a packet is received. */
    boost::signals2::signal<void (const OString&)> sigPacketReceived;

    typedef void          (*FileSentCallback)( bool aSuccess, void* pUserData);
    TUBES_DLLPUBLIC void    sendFile( rtl::OUString &localUri, FileSentCallback pCallback, void* pUserData);
    TUBES_DLLPUBLIC const OString& getUuid() const { return msUuid; }
    void                    setUuid( const OString& rUuid ) { msUuid = rUuid; }

    // --- following only to be called only by manager's callbacks ---
    // TODO: make friends instead

    void                    setChannel( TpAccount* pAccount, TpDBusTubeChannel* pChannel );
    TpDBusTubeChannel*      getChannel() const  { return mpChannel; }
    bool                    offerTube();
    bool                    acceptTube();
    bool                    setTube( GDBusConnection* pTube );
    /// got tube accepted on other end as well?
    bool                    isTubeOpen() const { return mpTube != NULL; }

    // Only for callbacks.
    void                    setTubeOfferedHandlerInvoked( bool b ) { mbTubeOfferedHandlerInvoked = b; }
    bool                    isTubeOfferedHandlerInvoked() const { return mbTubeOfferedHandlerInvoked; }

private:
    typedef ::std::queue<OString> TelePacketQueue;
    bool                    spinUntilTubeEstablished();

    TeleManager*            mpManager;
    TpAccount*              mpAccount;
    TpDBusTubeChannel*      mpChannel;
    OString                 msUuid;
    gchar*                  mpAddress;
    GDBusConnection*        mpTube;
    guint                   maObjectRegistrationId;
    TelePacketQueue         maPacketQueue;

    bool                    mbTubeOfferedHandlerInvoked : 1;
};

#endif // INCLUDED_TUBES_CONFERENCE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
