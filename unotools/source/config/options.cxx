/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: options.cxx,v $
 * $Revision: 1.5 $
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
#include "precompiled_unotools.hxx"

#include "sal/config.h"
#include <tools/list.hxx>
#include <unotools/options.hxx>

namespace utl
{
    DECLARE_LIST( IMPL_ConfigurationListenerList, ConfigurationListener* );
}

using utl::detail::Options;
using utl::ConfigurationBroadcaster;

ConfigurationBroadcaster::ConfigurationBroadcaster()
: mpList(0)
, m_nBroadcastBlocked( 0 )
, m_nBlockedHint( 0 )
{
}

ConfigurationBroadcaster::~ConfigurationBroadcaster()
{
    delete mpList;
}

void ConfigurationBroadcaster::AddListener( utl::ConfigurationListener* pListener )
{
    if ( !mpList )
        mpList = new IMPL_ConfigurationListenerList;
    mpList->Insert( pListener );
}

void ConfigurationBroadcaster::RemoveListener( utl::ConfigurationListener* pListener )
{
    if ( mpList )
        mpList->Remove( pListener );
}

void ConfigurationBroadcaster::NotifyListeners( sal_uInt32 nHint )
{
    if ( m_nBroadcastBlocked )
        m_nBlockedHint |= nHint;
    else
    {
        nHint |= m_nBlockedHint;
        m_nBlockedHint = 0;
        if ( mpList )
            for ( sal_uInt32 n=0; n<mpList->Count(); n++ )
                mpList->GetObject(n)->ConfigurationChanged( this, nHint );
    }
}

void ConfigurationBroadcaster::BlockBroadcasts( bool bBlock )
{
    if ( bBlock )
        ++m_nBroadcastBlocked;
    else if ( m_nBroadcastBlocked )
    {
        if ( --m_nBroadcastBlocked == 0 )
            NotifyListeners( 0 );
    }
}

Options::Options()
{
}

Options::~Options()
{
}

void Options::ConfigurationChanged( ConfigurationBroadcaster*, sal_uInt32 nHint )
{
    NotifyListeners( nHint );
}


