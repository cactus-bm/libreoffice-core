/*************************************************************************
 *
 *  $RCSfile: cmdbasestream.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: mh $ $Date: 2002-11-18 15:27:30 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2002
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2002 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2002 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

/*************************************************************************
 *
 *    ATTENTION
 *    This file is intended to work inside and outside the StarOffice environment.
 *    Only adaption of file commtypes.hxx should be necessary. Else it is a bug!
 *
 ************************************************************************/


#include "cmdbasestream.hxx"
#include "rcontrol.hxx"

CmdBaseStream::CmdBaseStream()
: pCommStream( NULL )
{
}

CmdBaseStream::~CmdBaseStream()
{
}

void CmdBaseStream::GenError (comm_ULONG nError, const comm_UniChar* aString, comm_USHORT nLenInChars )
{
    Write(comm_USHORT(SIReturnError));
    Write(nError);
    Write(aString, nLenInChars);
}

void CmdBaseStream::GenReturn (comm_USHORT nRet, comm_ULONG nUId )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(nUId);
    Write(comm_USHORT(PARAM_NONE));             // Typ der folgenden Parameter
}

void CmdBaseStream::GenReturn (comm_USHORT nRet, comm_ULONG nUId, comm_ULONG nNr )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(nUId);
    Write(comm_USHORT(PARAM_ULONG_1));          // Typ der folgenden Parameter
    Write(nNr);
}

void CmdBaseStream::GenReturn (comm_USHORT nRet, comm_ULONG nUId, const comm_UniChar* aString, comm_USHORT nLenInChars )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(nUId);
    Write(comm_USHORT(PARAM_STR_1));                // Typ der folgenden Parameter
    Write(aString, nLenInChars);
}

void CmdBaseStream::GenReturn (comm_USHORT nRet, comm_ULONG nUId, comm_BOOL bBool )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(nUId);
    Write(comm_USHORT(PARAM_BOOL_1));           // Typ der folgenden Parameter
    Write(bBool);
}

void CmdBaseStream::GenReturn (comm_USHORT nRet, comm_ULONG nUId, comm_ULONG nNr, const comm_UniChar* aString, comm_USHORT nLenInChars, comm_BOOL bBool )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(nUId);
    Write(comm_USHORT(PARAM_ULONG_1|PARAM_STR_1|PARAM_BOOL_1));     // Typ der folgenden Parameter
    Write(nNr);
    Write(aString, nLenInChars);
    Write(bBool);
}

// MacroRecorder
void CmdBaseStream::GenReturn( comm_USHORT nRet, comm_ULONG nUId, comm_USHORT nMethod )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(nUId);
    Write(comm_USHORT(PARAM_USHORT_1));     // Typ der folgenden Parameter
    Write(nMethod);
}

void CmdBaseStream::GenReturn( comm_USHORT nRet, comm_ULONG nUId, comm_USHORT nMethod, const comm_UniChar* aString, comm_USHORT nLenInChars )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(nUId);
    Write(comm_USHORT(PARAM_USHORT_1|PARAM_STR_1));     // Typ der folgenden Parameter
    Write(nMethod);
    Write(aString, nLenInChars);
}

void CmdBaseStream::GenReturn( comm_USHORT nRet, comm_ULONG nUId, comm_USHORT nMethod, const comm_UniChar* aString, comm_USHORT nLenInChars, comm_BOOL bBool )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(nUId);
    Write(comm_USHORT(PARAM_USHORT_1|PARAM_STR_1|PARAM_BOOL_1));        // Typ der folgenden Parameter
    Write(nMethod);
    Write(aString, nLenInChars);
    Write(bBool);
}

void CmdBaseStream::GenReturn( comm_USHORT nRet, comm_ULONG nUId, comm_USHORT nMethod, comm_BOOL bBool )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(nUId);
    Write(comm_USHORT(PARAM_USHORT_1|PARAM_BOOL_1));        // Typ der folgenden Parameter
    Write(nMethod);
    Write(bBool);
}

void CmdBaseStream::GenReturn( comm_USHORT nRet, comm_ULONG nUId, comm_USHORT nMethod, comm_ULONG nNr )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(nUId);
    Write(comm_USHORT(PARAM_USHORT_1|PARAM_ULONG_1));       // Typ der folgenden Parameter
    Write(nMethod);
    Write(nNr);
}




// StringID
void CmdBaseStream::GenReturn( comm_USHORT nRet, const comm_UniChar* aUId, comm_USHORT nUIdLenInChars )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(aUId, nUIdLenInChars);
    Write(comm_USHORT(PARAM_NONE));             // Typ der folgenden Parameter
}

void CmdBaseStream::GenReturn( comm_USHORT nRet, const comm_UniChar* aUId, comm_USHORT nUIdLenInChars, comm_ULONG nNr )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(aUId, nUIdLenInChars);
    Write(comm_USHORT(PARAM_ULONG_1));          // Typ der folgenden Parameter
    Write(nNr);
}

void CmdBaseStream::GenReturn( comm_USHORT nRet, const comm_UniChar* aUId, comm_USHORT nUIdLenInChars, const comm_UniChar* aString, comm_USHORT nLenInChars )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(aUId, nUIdLenInChars);
    Write(comm_USHORT(PARAM_STR_1));                // Typ der folgenden Parameter
    Write(aString, nLenInChars);
}

void CmdBaseStream::GenReturn( comm_USHORT nRet, const comm_UniChar* aUId, comm_USHORT nUIdLenInChars, comm_BOOL bBool )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(aUId, nUIdLenInChars);
    Write(comm_USHORT(PARAM_BOOL_1));           // Typ der folgenden Parameter
    Write(bBool);
}

void CmdBaseStream::GenReturn( comm_USHORT nRet, const comm_UniChar* aUId, comm_USHORT nUIdLenInChars, comm_ULONG nNr, const comm_UniChar* aString, comm_USHORT nLenInChars, comm_BOOL bBool )
{
    Write(comm_USHORT(SIReturn));
    Write(nRet);
    Write(aUId, nUIdLenInChars);
    Write(comm_USHORT(PARAM_ULONG_1|PARAM_STR_1|PARAM_BOOL_1));     // Typ der folgenden Parameter
    Write(nNr);
    Write(aString, nLenInChars);
    Write(bBool);
}


void CmdBaseStream::Read (comm_USHORT &nNr)
{
    comm_USHORT nId;
    *pCommStream >> nId;
    if (pCommStream->IsEof()) return;
    if (nId != BinUSHORT) DBG_ERROR1( "Falscher Typ im Stream: Erwartet USHORT, gefunden :%hu", nId );
    *pCommStream >> nNr;
}

void CmdBaseStream::Read (comm_ULONG &nNr)
{
    comm_USHORT nId;
    *pCommStream >> nId;
    if (pCommStream->IsEof()) return;
    if (nId != BinULONG) DBG_ERROR1( "Falscher Typ im Stream: Erwartet ULONG, gefunden :%hu", nId );
    *pCommStream >> nNr;
}

void CmdBaseStream::Read (comm_UniChar* &aString, comm_USHORT &nLenInChars )
{
    comm_USHORT nId;
    *pCommStream >> nId;
    if (nId != BinString) DBG_ERROR1( "Falscher Typ im Stream: Erwartet String, gefunden :%hu", nId );

    *pCommStream >> nLenInChars;

    aString = new comm_UniChar [nLenInChars];
    pCommStream->Read( aString, ((comm_ULONG)nLenInChars) * sizeof( comm_UniChar ) );
#ifdef __BIGENDIAN
    // we have to change the byteorder
    comm_USHORT n;
    for ( n = 0 ; n < nLenInChars ; n++ )
        aString[ n ] = aString[ n ] >> 8 | aString[ n ] << 8;
#endif
}

void CmdBaseStream::Read (comm_BOOL &bBool)
{
    comm_USHORT nId;
    *pCommStream >> nId;
    if (nId != BinBool) DBG_ERROR1( "Falscher Typ im Stream: Erwartet BOOL, gefunden :%hu", nId );
    *pCommStream >> bBool;
}

comm_USHORT CmdBaseStream::GetNextType()
{
    comm_USHORT nId;
    *pCommStream >> nId;
    pCommStream->SeekRel(-2);
    return nId;
}


void CmdBaseStream::Write( comm_USHORT nNr )
{
    *pCommStream << comm_USHORT( BinUSHORT );
    *pCommStream << nNr;
}

void CmdBaseStream::Write( comm_ULONG nNr )
{
    *pCommStream << comm_USHORT( BinULONG );
    *pCommStream << nNr;
}

void CmdBaseStream::Write( const comm_UniChar* aString, comm_USHORT nLenInChars )
{
    *pCommStream << comm_USHORT(BinString);
    *pCommStream << nLenInChars;
#ifdef __BIGENDIAN
    // we have to change the byteorder
    comm_UniChar* aNewString;
    aNewString = new comm_UniChar [nLenInChars];
    comm_USHORT n;
    for ( n = 0 ; n < nLenInChars ; n++ )
        aNewString[ n ] = aString[ n ] >> 8 | aString[ n ] << 8;
    pCommStream->Write( aNewString, ((comm_ULONG)nLenInChars) * sizeof( comm_UniChar ) );
    delete [] aNewString;
#else
    pCommStream->Write( aString, ((comm_ULONG)nLenInChars) * sizeof( comm_UniChar ) );
#endif
}

void CmdBaseStream::Write( comm_BOOL bBool )
{
    *pCommStream << comm_USHORT( BinBool );
    *pCommStream << bBool;
}

