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
#include <osl/endian.h>
#include <osl/diagnose.h>

#include "cmdbasestream.hxx"
#include "rcontrol.hxx"

CmdBaseStream::CmdBaseStream()
: pCommStream( NULL )
{
}

CmdBaseStream::~CmdBaseStream()
{
}

void CmdBaseStream::GenError (rtl::OString *pUId, comm_String *pString )
{
    Write(comm_UINT16(SIReturnError));
    Write(pUId);
    Write(pString);
}

void CmdBaseStream::GenReturn (comm_UINT16 nRet, comm_UINT32 nUId )
{
    Write(comm_UINT16(SIReturn));
    Write(nRet);
    Write(nUId);
    Write(comm_UINT16(PARAM_NONE));
}

void CmdBaseStream::GenReturn (comm_UINT16 nRet, rtl::OString *pUId, comm_UINT32 nNr )
{
    Write(comm_UINT16(SIReturn));
    Write(nRet);
    if (pUId->equalsL(RTL_CONSTASCII_STRINGPARAM("UID_ACTIVE")))
        Write(comm_UINT32(0));
    else
        Write(pUId);
    Write(comm_UINT16(PARAM_UINT32_1));
    Write(nNr);
}

void CmdBaseStream::GenReturn (comm_UINT16 nRet, rtl::OString *pUId, comm_String *pString )
{
    Write(comm_UINT16(SIReturn));
    Write(nRet);
    if (pUId->equalsL(RTL_CONSTASCII_STRINGPARAM("UID_ACTIVE")))
        Write(comm_UINT32(0));
    else
        Write(pUId);
    Write(comm_UINT16(PARAM_STR_1));
    Write(pString);
}

void CmdBaseStream::GenReturn (comm_UINT16 nRet, rtl::OString *pUId, comm_BOOL bBool )
{
    Write(comm_UINT16(SIReturn));
    Write(nRet);
    if (pUId->equalsL(RTL_CONSTASCII_STRINGPARAM("UID_ACTIVE")))
        Write(comm_UINT32(0));
    else
        Write(pUId);
    Write(comm_UINT16(PARAM_BOOL_1));
    Write(bBool);
}

void CmdBaseStream::GenReturn (comm_UINT16 nRet, rtl::OString *pUId, comm_UINT32 nNr, comm_String *pString, comm_BOOL bBool )
{
    Write(comm_UINT16(SIReturn));
    Write(nRet);
    if (pUId->equalsL(RTL_CONSTASCII_STRINGPARAM("UID_ACTIVE")))
        Write(comm_UINT32(0));
    else
        Write(pUId);
    Write(comm_UINT16(PARAM_UINT32_1|PARAM_STR_1|PARAM_BOOL_1));
    Write(nNr);
    Write(pString);
    Write(bBool);
}

void CmdBaseStream::GenReturn (comm_UINT16 nRet, comm_UINT16 nMethod, comm_UINT32 nNr )
{
    Write(comm_UINT16(SIReturn));
    Write(nRet);
    Write((comm_UINT32)nMethod); //HELPID BACKWARD (no sal_uLong needed)
    Write(comm_UINT16(PARAM_UINT32_1));
    Write(nNr);
}

void CmdBaseStream::GenReturn (comm_UINT16 nRet, comm_UINT16 nMethod, comm_String *pString )
{
    Write(comm_UINT16(SIReturn));
    Write(nRet);
    Write((comm_UINT32)nMethod); //HELPID BACKWARD (no sal_uLong needed)
    Write(comm_UINT16(PARAM_STR_1));
    Write(pString);
}

void CmdBaseStream::GenReturn (comm_UINT16 nRet, comm_UINT16 nMethod, comm_BOOL bBool )
{
    Write(comm_UINT16(SIReturn));
    Write(nRet);
    Write((comm_UINT32)nMethod); //HELPID BACKWARD (no sal_uLong needed)
    Write(comm_UINT16(PARAM_BOOL_1));
    Write(bBool);
}

void CmdBaseStream::GenReturn (comm_UINT16 nRet, comm_UINT16 nMethod, comm_UINT16 nNr )
{
    Write(comm_UINT16(SIReturn));
    Write(nRet);
    Write((comm_UINT32)nMethod); //HELPID BACKWARD (no sal_uLong needed)
    Write(comm_UINT16(PARAM_UINT16_1));
    Write(nNr);
}


// MacroRecorder
void CmdBaseStream::GenReturn( comm_UINT16 nRet, rtl::OString *pUId, comm_UINT16 nMethod )
{
    Write(comm_UINT16(SIReturn));
    Write(nRet);
    Write(pUId);
    Write(comm_UINT16(PARAM_UINT16_1));
    Write(nMethod);
}

void CmdBaseStream::GenReturn( comm_UINT16 nRet, rtl::OString *pUId, comm_UINT16 nMethod, comm_String *pString )
{
    Write(comm_UINT16(SIReturn));
    Write(nRet);
    Write(pUId);
    Write(comm_UINT16(PARAM_UINT16_1|PARAM_STR_1));
    Write(nMethod);
    Write(pString);
}

void CmdBaseStream::GenReturn( comm_UINT16 nRet, rtl::OString *pUId, comm_UINT16 nMethod, comm_String *pString, comm_BOOL bBool )
{
    Write(comm_UINT16(SIReturn));
    Write(nRet);
    Write(pUId);
    Write(comm_UINT16(PARAM_UINT16_1|PARAM_STR_1|PARAM_BOOL_1));
    Write(nMethod);
    Write(pString);
    Write(bBool);
}

void CmdBaseStream::GenReturn( comm_UINT16 nRet, rtl::OString *pUId, comm_UINT16 nMethod, comm_UINT32 nNr )
{
    Write(comm_UINT16(SIReturn));
    Write(nRet);
    Write(pUId);
    Write(comm_UINT16(PARAM_UINT16_1|PARAM_UINT32_1));
    Write(nMethod);
    Write(nNr);
}



void CmdBaseStream::Read (comm_UINT16 &nNr)
{
    comm_UINT16 nId;
    *pCommStream >> nId;
    if (pCommStream->IsEof()) return;
#if OSL_DEBUG_LEVEL > 1
    if (nId != BinUINT16) OSL_TRACE( "Falscher Typ im Stream: Erwartet USHORT, gefunden :%hu", nId );
#endif
    *pCommStream >> nNr;
}

void CmdBaseStream::Read (comm_UINT32 &nNr)
{
    comm_UINT16 nId;
    *pCommStream >> nId;
    if (pCommStream->IsEof()) return;
#if OSL_DEBUG_LEVEL > 1
    if (nId != BinUINT32) OSL_TRACE( "Falscher Typ im Stream: Erwartet ULONG, gefunden :%hu", nId );
#endif
    *pCommStream >> nNr;
}

void CmdBaseStream::Read (comm_UniChar* &aString, comm_UINT16 &nLenInChars )
{
    comm_UINT16 nId;
    *pCommStream >> nId;
#if OSL_DEBUG_LEVEL > 1
    if (nId != BinString) OSL_TRACE( "Falscher Typ im Stream: Erwartet String, gefunden :%hu", nId );
#endif

    *pCommStream >> nLenInChars;

    aString = new comm_UniChar [nLenInChars];
    pCommStream->Read( aString, ((comm_UINT32)nLenInChars) * sizeof( comm_UniChar ) );
#ifdef OSL_BIGENDIAN
    // we have to change the byteorder
    comm_UINT16 n;
    for ( n = 0 ; n < nLenInChars ; n++ )
        aString[ n ] = aString[ n ] >> 8 | aString[ n ] << 8;
#endif
}

void CmdBaseStream::Read (comm_BOOL &bBool)
{
    comm_UINT16 nId;
    *pCommStream >> nId;
#if OSL_DEBUG_LEVEL > 1
    if (nId != BinBool) OSL_TRACE( "Falscher Typ im Stream: Erwartet BOOL, gefunden :%hu", nId );
#endif
    *pCommStream >> bBool;
}

comm_UINT16 CmdBaseStream::GetNextType()
{
    comm_UINT16 nId;
    *pCommStream >> nId;
    pCommStream->SeekRel(-2);
    return nId;
}


void CmdBaseStream::Write( comm_UINT16 nNr )
{
    *pCommStream << comm_UINT16( BinUINT16 );
    *pCommStream << nNr;
}

void CmdBaseStream::Write( comm_UINT32 nNr )
{
    *pCommStream << comm_UINT16( BinUINT32 );
    *pCommStream << nNr;
}

void CmdBaseStream::Write( const comm_UniChar* aString, comm_UINT16 nLenInChars )
{
    *pCommStream << comm_UINT16(BinString);

    comm_UINT16 n;

    // remove BiDi and zero-width-markers    0x200B - 0x200F
    // remove BiDi and paragraph-markers     0x2028 - 0x202E

    comm_UniChar* aNoBiDiString;
    aNoBiDiString = new comm_UniChar [nLenInChars];
    comm_UINT16 nNewLenInChars = 0;
    for ( n = 0 ; n < nLenInChars ; n++ )
    {
        comm_UniChar c = aString[ n ];
        if (  ((c >= 0x200B) && (c <= 0x200F))
            ||((c >= 0x2028) && (c <= 0x202E)) )
        {   //Ignore character
        }
        else
        {
            aNoBiDiString[ nNewLenInChars ] = c;
            nNewLenInChars++;
        }
    }

    *pCommStream << nNewLenInChars;

#ifdef OSL_BIGENDIAN
    // we have to change the byteorder
    comm_UniChar* aNewString;
    aNewString = new comm_UniChar [nNewLenInChars];
    for ( n = 0 ; n < nNewLenInChars ; n++ )
        aNewString[ n ] = aNoBiDiString[ n ] >> 8 | aNoBiDiString[ n ] << 8;
    pCommStream->Write( aNewString, ((comm_UINT32)nNewLenInChars) * sizeof( comm_UniChar ) );
    delete [] aNewString;
#else
    pCommStream->Write( aNoBiDiString, ((comm_UINT32)nNewLenInChars) * sizeof( comm_UniChar ) );
#endif

    delete [] aNoBiDiString;
}

void CmdBaseStream::Write( comm_BOOL bBool )
{
    *pCommStream << comm_UINT16( BinBool );
    *pCommStream << bBool;
}

void CmdBaseStream::Read ( comm_String* &pString )
{
    (void) pString; /* avoid warning about unused parameter */
    OSL_FAIL("Read ( comm_String* &pString ) Not Implemented");
}
void CmdBaseStream::Read ( rtl::OString* &pId )
{
    (void) pId; /* avoid warning about unused parameter */
    OSL_FAIL("Read ( rtl::OString* &pId ) Not Implemented");
}

void CmdBaseStream::Write( comm_String *pString )
{
    (void) pString; /* avoid warning about unused parameter */
    OSL_FAIL("Write( comm_String *pString ) Not Implemented");
}
void CmdBaseStream::Write( rtl::OString* pId )
{
    (void) pId; /* avoid warning about unused parameter */
    OSL_FAIL("Write( rtl::OString* pId ) Not Implemented");
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
