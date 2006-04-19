/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: scmdstrm.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: hr $ $Date: 2006-04-19 13:58:35 $
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
#ifndef _SFXINTITEM_HXX //autogen
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif

#ifndef _SCMDSTRM_HXX
#include "scmdstrm.hxx"
#endif
#include "svcommstream.hxx"
#include "rcontrol.hxx"

#if OSL_DEBUG_LEVEL > 1
#ifndef _EDITWIN_HXX
#include "editwin.hxx"
#endif
#include "statemnt.hxx"
#endif

SCmdStream::SCmdStream(SvStream *pIn)
{
    pSammel = pIn;
    pCommStream = new SvCommStream( pSammel );
//  SetCommStream( pCommStream );
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
    USHORT nLenInChars;
    CmdBaseStream::Read( pStr, nLenInChars );

    *pString = String( pStr, nLenInChars );
    delete [] pStr;
}

void SCmdStream::Read (String &aString)
{
    comm_UniChar* pStr;
    USHORT nLenInChars;
    CmdBaseStream::Read( pStr, nLenInChars );

    aString = String( pStr, nLenInChars );
    delete [] pStr;
}

void SCmdStream::Read ( SfxPoolItem *&pItem )
{
    USHORT nType;
    USHORT nId;
    Read(nId);
#if OSL_DEBUG_LEVEL > 1
        StatementList::m_pDbgWin->AddText( "Parameter: " );
        StatementList::m_pDbgWin->AddText( String::CreateFromInt32( nId ) );
        StatementList::m_pDbgWin->AddText( " " );
#endif
    Read( nType );
    switch (nType)
    {
        case BinUSHORT:
            {
                comm_USHORT nNr;
                Read (nNr );
                pItem = new SfxUInt16Item(nId,nNr);
#if OSL_DEBUG_LEVEL > 1
                StatementList::m_pDbgWin->AddText( "USHORT:" );
                StatementList::m_pDbgWin->AddText( String::CreateFromInt32( nNr ) );
#endif
            }
            break;
        case BinULONG:
            {
                comm_ULONG nNr;
                Read (nNr );
                pItem = new SfxUInt32Item(nId,nNr);
#if OSL_DEBUG_LEVEL > 1
                StatementList::m_pDbgWin->AddText( "ULONG:" );
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
            DBG_ERROR1( "Ung�ltiger Typ im Stream:%hu", nType );
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
    USHORT nType;
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
        case BinUSHORT:
            {
                comm_USHORT nNr;
                Read (nNr );
                rItem.Value <<= nNr;
#if OSL_DEBUG_LEVEL > 1
                StatementList::m_pDbgWin->AddText( "USHORT:" );
                StatementList::m_pDbgWin->AddText( String::CreateFromInt32( nNr ) );
#endif
            }
            break;
        case BinULONG:
            {
                comm_ULONG nNr;
                Read (nNr );
                rItem.Value <<= nNr;
#if OSL_DEBUG_LEVEL > 1
                StatementList::m_pDbgWin->AddText( "ULONG:" );
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
            DBG_ERROR1( "Ung�ltiger Typ im Stream:%hu", nType );
#if OSL_DEBUG_LEVEL > 1
            StatementList::m_pDbgWin->AddText( "Ung�ltiger Typ !!!! " );
#endif
            break;
    }
#if OSL_DEBUG_LEVEL > 1
        StatementList::m_pDbgWin->AddText( "\n" );
#endif
}

