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

#include <svl/intitem.hxx>
#include <svl/stritem.hxx>
#include <svl/eitem.hxx>
#include "scmdstrm.hxx"
#include "svcommstream.hxx"
#include "rcontrol.hxx"

#if OSL_DEBUG_LEVEL > 1
#include "editwin.hxx"
#include "statemnt.hxx"
#endif

SCmdStream::SCmdStream(SvStream *pIn)
{
    pSammel = pIn;
    pCommStream = new SvCommStream( pSammel );
}

SCmdStream::~SCmdStream()
{
    delete pCommStream;
}

void SCmdStream::Read (String* &pString)
{
    if ( !pString )
        pString = new String();
    comm_UniChar* pStr;
    sal_uInt16 nLenInChars;
    CmdBaseStream::Read( pStr, nLenInChars );

    *pString = String( pStr, nLenInChars );
    delete [] pStr;
}

void SCmdStream::Read (String &aString)
{
    comm_UniChar* pStr;
    sal_uInt16 nLenInChars;
    CmdBaseStream::Read( pStr, nLenInChars );

    aString = String( pStr, nLenInChars );
    delete [] pStr;
}

void SCmdStream::Read ( SfxPoolItem *&pItem )
{
    sal_uInt16 nType;
    sal_uInt16 nId;
    Read(nId);
#if OSL_DEBUG_LEVEL > 1
        StatementList::m_pDbgWin->AddText( "Parameter: " );
        StatementList::m_pDbgWin->AddText( String::CreateFromInt32( nId ) );
        StatementList::m_pDbgWin->AddText( " " );
#endif
    Read( nType );
    switch (nType)
    {
        case BinUINT16:
            {
                comm_UINT16 nNr;
                Read (nNr );
                pItem = new SfxUInt16Item(nId,nNr);
#if OSL_DEBUG_LEVEL > 1
                StatementList::m_pDbgWin->AddText( "UINT16" );
                StatementList::m_pDbgWin->AddText( String::CreateFromInt32( nNr ) );
#endif
            }
            break;
        case BinUINT32:
            {
                comm_UINT32 nNr;
                Read (nNr );
                pItem = new SfxUInt32Item(nId,nNr);
#if OSL_DEBUG_LEVEL > 1
                StatementList::m_pDbgWin->AddText( "UINT32" );
                StatementList::m_pDbgWin->AddText( String::CreateFromInt64( nNr ) );
#endif
            }
            break;
        case BinString:
            {
                String aString;
                Read (aString);

                pItem = new SfxStringItem(nId,aString);
#if OSL_DEBUG_LEVEL > 1
                StatementList::m_pDbgWin->AddText( "String:" );
                StatementList::m_pDbgWin->AddText( aString );
#endif
            }
            break;
        case BinBool:
            {
                comm_BOOL bBool;
                Read (bBool);
                pItem = new SfxBoolItem(nId,bBool);
#if OSL_DEBUG_LEVEL > 1
                StatementList::m_pDbgWin->AddText( "BOOL:" );
                StatementList::m_pDbgWin->AddText( bBool ? "TRUE" : "FALSE" );
#endif
            }
            break;
        default:
            OSL_TRACE( "Ung�ltiger Typ im Stream:%hu", nType );
#if OSL_DEBUG_LEVEL > 1
            StatementList::m_pDbgWin->AddText( "Ung�ltiger Typ !!!! " );
#endif
            break;
    }
#if OSL_DEBUG_LEVEL > 1
        StatementList::m_pDbgWin->AddText( "\n" );
#endif
}

void SCmdStream::Read ( ::com::sun::star::beans::PropertyValue &rItem )
{
    sal_uInt16 nType;
    String aId;
    Read(aId);
    rItem.Name = rtl::OUString( aId );
#if OSL_DEBUG_LEVEL > 1
        StatementList::m_pDbgWin->AddText( "Parameter: " );
        StatementList::m_pDbgWin->AddText( aId );
        StatementList::m_pDbgWin->AddText( " " );
#endif
    nType = GetNextType();
    switch (nType)
    {
        case BinUINT16:
            {
                comm_UINT16 nNr;
                Read (nNr );
                rItem.Value <<= nNr;
#if OSL_DEBUG_LEVEL > 1
                StatementList::m_pDbgWin->AddText( "UINT16" );
                StatementList::m_pDbgWin->AddText( String::CreateFromInt32( nNr ) );
#endif
            }
            break;
        case BinUINT32:
            {
                comm_UINT32 nNr;
                Read (nNr );
                rItem.Value <<= nNr;
#if OSL_DEBUG_LEVEL > 1
                StatementList::m_pDbgWin->AddText( "UINT32" );
                StatementList::m_pDbgWin->AddText( String::CreateFromInt64( nNr ) );
#endif
            }
            break;
        case BinString:
            {
                String aString;
                Read (aString);
                rItem.Value <<= ::rtl::OUString( aString );
#if OSL_DEBUG_LEVEL > 1
                StatementList::m_pDbgWin->AddText( "String:" );
                StatementList::m_pDbgWin->AddText( aString );
#endif
            }
            break;
        case BinBool:
            {
                comm_BOOL bBool;
                Read (bBool);
                rItem.Value <<= bBool;
#if OSL_DEBUG_LEVEL > 1
                StatementList::m_pDbgWin->AddText( "BOOL:" );
                StatementList::m_pDbgWin->AddText( bBool ? "TRUE" : "FALSE" );
#endif
            }
            break;
        default:
            OSL_TRACE( "Ung�ltiger Typ im Stream:%hu", nType );
#if OSL_DEBUG_LEVEL > 1
            StatementList::m_pDbgWin->AddText( "Ung�ltiger Typ !!!! " );
#endif
            break;
    }
#if OSL_DEBUG_LEVEL > 1
        StatementList::m_pDbgWin->AddText( "\n" );
#endif
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
