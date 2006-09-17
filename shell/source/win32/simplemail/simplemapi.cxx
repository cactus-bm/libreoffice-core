/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: simplemapi.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 01:47:45 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_shell.hxx"

#ifndef INCLUDED_SIMPLEMAPI_HXX
#include "simplemapi.hxx"
#endif

#include <string>
#include <stdexcept>

CSimpleMapi::CSimpleMapi() :
    m_lpfnMapiLogon(NULL),
    m_lpfnMapiLogoff(NULL),
    m_lpfnMapiSendMail(NULL)
{
    m_hMapiDll = LoadLibrary("mapi32.dll");
    if ((m_hMapiDll == INVALID_HANDLE_VALUE) || (m_hMapiDll == NULL))
        throw std::runtime_error("Couldn't load MAPI library");

    m_lpfnMapiLogon = reinterpret_cast<LPMAPILOGON>(GetProcAddress(m_hMapiDll, "MAPILogon"));
    if (!m_lpfnMapiLogon)
        throw std::runtime_error("Couldn't find method MAPILogon");

    m_lpfnMapiLogoff = reinterpret_cast<LPMAPILOGOFF>(GetProcAddress(m_hMapiDll, "MAPILogoff"));
    if (!m_lpfnMapiLogoff)
        throw std::runtime_error("Couldn't find method MAPILogoff");

    m_lpfnMapiSendMail = reinterpret_cast<LPMAPISENDMAIL>(GetProcAddress(m_hMapiDll, "MAPISendMail"));
    if (!m_lpfnMapiSendMail)
        throw std::runtime_error("Couldn't find method MAPISendMail");
}

CSimpleMapi::~CSimpleMapi()
{
    FreeLibrary(m_hMapiDll);
}

ULONG CSimpleMapi::MAPILogon(
    ULONG ulUIParam,
    LPTSTR lpszProfileName,
    LPTSTR lpszPassword,
    FLAGS flFlags,
    ULONG ulReserved,
    LPLHANDLE lplhSession )
{
    return m_lpfnMapiLogon(
        ulUIParam,
        lpszProfileName,
        lpszPassword,
        flFlags,
        ulReserved,
        lplhSession );
}

ULONG CSimpleMapi::MAPILogoff(
    LHANDLE lhSession,
    ULONG ulUIParam,
    FLAGS flFlags,
    ULONG ulReserved )
{
    return m_lpfnMapiLogoff(lhSession, ulUIParam, flFlags, ulReserved);
}

ULONG CSimpleMapi::MAPISendMail(
    LHANDLE lhSession,
    ULONG ulUIParam,
    lpMapiMessage lpMessage,
    FLAGS flFlags,
    ULONG ulReserved )
{
    return m_lpfnMapiSendMail(lhSession, ulUIParam, lpMessage, flFlags, ulReserved);
}

