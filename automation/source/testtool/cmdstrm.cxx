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

#include <vcl/keycod.hxx>
#include <basic/sbx.hxx>
#include <tools/stream.hxx>

#include "cmdstrm.hxx"
#include "rcontrol.hxx"
#include "objtest.hxx"
#include "ittresid.hxx"
#include "svcommstream.hxx"
#include <basic/testtool.hrc>

ControlDefLoad const CmdStream::arKeyCodes [] =
#include <keycodes.hxx>
CNames *CmdStream::pKeyCodes = NULL;

ResMgr* IttResId::getIttResMgr()
{
    static ResMgr* pMgr = NULL;
    if( ! pMgr )
        pMgr = CREATEVERSIONRESMGR( stt );
    return pMgr;
}

CmdStream::CmdStream()
{
    pSammel = new SvMemoryStream();
    pCommStream = new SvCommStream( pSammel );
}

CmdStream::~CmdStream()
{
    delete pCommStream;
    delete pSammel;
}


#define PUT_CHAR(ch) \
        sal_uInt16 nMCode = nModify + ch;                       \
        sal_uInt16 _Code = 1;                                   \
        if ( (nMCode & 0xFF) == 0 )                         \
            _Code |= 2;                                     \
        if ( (nMCode >> 8) == 0 )                           \
            _Code |= 4;                                     \
        Result += (char) _Code;                             \
        Result += (char) ( nMCode & 0xFF );                 \
        Result += (char) ( nMCode >> 8 )


String CmdStream::WandleKeyEventString( String aKeys )
{
    if ( !pKeyCodes )
        ReadFlatArray(arKeyCodes,pKeyCodes);
    if ( !pKeyCodes )
        return aKeys;

    xub_StrLen nPos1 = 0;
    while ( (nPos1 = aKeys.Search('<',nPos1)) != STRING_NOTFOUND)
    {
        xub_StrLen nPos2 = aKeys.Search('>',nPos1);
        if ( nPos2 != STRING_NOTFOUND )
        {
            String Work = aKeys.Copy(nPos1+1,nPos2-nPos1+1-2);
            aKeys.Erase(nPos1,nPos2-nPos1+1);   // includes removing <>'s
            String Result, Token;
            sal_uInt16 nModify = 0;
            while ( Work.Len() > 0 )
            {
                Token = Work.GetToken(0,' ');
                Work.Erase(0,Token.Len()+1);
                ControlDef WhatName(Token,rtl::OString());
                sal_uInt16 nElement;
                if (pKeyCodes->Seek_Entry(&WhatName,&nElement))
                {
                    // FIXME: HELPID
                    sal_uInt16 nCode = 0;//(sal_uInt16) pKeyCodes->GetObject(nElement)->pData->aUId.GetNum();
                    if ( nCode >= KEY_SHIFT )
                        nModify ^= nCode;
                    else
                    {
                        if ( ( nModify == 0 ) &&
                              (((nCode & 0xFF00) == KEYGROUP_NUM)   ||
                               ((nCode & 0xFF00) == KEYGROUP_ALPHA) ||
                               ( nCode == KEY_SPACE) ||
                               ( nCode == KEY_ADD) ||
                               ( nCode == KEY_SUBTRACT) ||
                               ( nCode == KEY_MULTIPLY) ||
                               ( nCode == KEY_DIVIDE) ||
                               ( nCode == KEY_POINT) ||
                               ( nCode == KEY_COMMA) ||
                               ( nCode == KEY_LESS) ||
                               ( nCode == KEY_GREATER) ||
                               ( nCode == KEY_EQUAL) ) )
                        {
                            switch ( nCode )
                            {
                                case KEY_SPACE:     Token = ' '; break;
                                case KEY_ADD:       Token = '+'; break;
                                case KEY_SUBTRACT:  Token = '-'; break;
                                case KEY_MULTIPLY:  Token = '*'; break;
                                case KEY_DIVIDE:    Token = '/'; break;
                                case KEY_POINT:     Token = '.'; break;
                                case KEY_COMMA:     Token = ','; break;
                                case KEY_LESS:      Token = '<'; break;
                                case KEY_GREATER:   Token = '>'; break;
                                case KEY_EQUAL:     Token = '='; break;
                                default:
//                                  if ( nModify == 0 )
//                                      Token.ToLower();
//                                  else
//                                      Token.ToUpper();
                                    ;
                            }
                            Result += Token;
                        }
                        else
                        {
                            PUT_CHAR (nCode);
                        }
                    }
                }
                else
                {
                    Result += '<';
                    Result += Token;
                    Result += ' ';
                    Result += String( IttResId( S_INVALID_KEYCODE ) );
                    Result += '>';
                }
            }
            aKeys.Insert(Result,nPos1);
            nPos1 = nPos1 + Result.Len();
        }
        else
            nPos1 = aKeys.Len() + 1;
    }

    return aKeys;
}


void CmdStream::WriteSortedParams( SbxArray* rPar, sal_Bool IsKeyString )
{
    sal_uInt16 nParams = PARAM_NONE;
    sal_uInt16 nNr1=0,nNr2=0,nNr3=0,nNr4=0;
    comm_UINT32 nLNr1=0;
    String aString1,aString2;
    sal_Bool bBool1=sal_False,bBool2=sal_False;

    if ( rPar )
    {
        for ( sal_uInt16 i = 1; i < rPar->Count() ; i++)
        {
            switch (rPar->Get( i )->GetType())
            {
                case SbxLONG:       // always pass everything as Short
                case SbxULONG:
                case SbxSALINT64:
                case SbxSALUINT64:
                case SbxDOUBLE:
                case SbxINTEGER:
                case SbxBYTE:
                case SbxUSHORT:
                case SbxINT:
                case SbxUINT:
                case SbxSINGLE:
                    if ( (nParams & PARAM_UINT16_1) == 0 )
                    {
                        nParams |= PARAM_UINT16_1;
                        nNr1 = rPar->Get( i )->GetUShort();
                    }
                    else if ( (nParams & PARAM_UINT16_2) == 0 )
                    {
                        nParams |= PARAM_UINT16_2;
                        nNr2 = rPar->Get( i )->GetUShort();
                    }
                    else if ( (nParams & PARAM_UINT16_3) == 0 )
                    {
                        nParams |= PARAM_UINT16_3;
                        nNr3 = rPar->Get( i )->GetUShort();
                    }
                    else if ( (nParams & PARAM_UINT16_4) == 0 )
                    {
                        nParams |= PARAM_UINT16_4;
                        nNr4 = rPar->Get( i )->GetUShort();
                    }
                    else
                        SbxBase::SetError( SbxERR_WRONG_ARGS );
                    break;
                case SbxOBJECT:     // whenever a control is passed. TabPage, MenuBar
                    {
                        SbxProperty *pMember = NULL;
                        if ( rPar->Get( i )->ISA( SbxObject ) )
                            pMember = ((SbxObject*)rPar->Get( i ))->GetDfltProperty();
                        if ( pMember != NULL )
                        {
                            if ( pMember->GetType() == SbxSTRING )
                            {
                                if ( nParams & PARAM_STR_1 )
                                    if ( nParams & PARAM_STR_2 )
                                        SbxBase::SetError( SbxERR_WRONG_ARGS );
                                    else
                                    {
                                        nParams |= PARAM_STR_2;
                                        aString2 = pMember->GetString();
                                    }
                                else
                                {
                                    nParams |= PARAM_STR_1;
                                    aString1 = pMember->GetString();
                                }
                                break;
                            }
                            else if ( pMember->GetType() == SbxULONG )
                            {
                                if ( nParams & PARAM_UINT32_1 )
                                    SbxBase::SetError( SbxERR_WRONG_ARGS );
                                else
                                {
                                    nParams |= PARAM_UINT32_1;
                                    nLNr1 = pMember->GetULong();
                                }
                            }
                            else
                                SbxBase::SetError( SbxERR_NAMED_NOT_FOUND );
                        }
                        else
                        {
                            SbxBase::SetError( SbxERR_NAMED_NOT_FOUND );
                        }
                    }
                    break;
                case SbxSTRING:
                case SbxCHAR:
                    if ( nParams & PARAM_STR_1 )
                        if ( nParams & PARAM_STR_2 )
                            SbxBase::SetError( SbxERR_WRONG_ARGS );
                        else
                        {
                            nParams |= PARAM_STR_2;
                            aString2 = rPar->Get( i )->GetString();
                        }
                    else
                    {
                        nParams |= PARAM_STR_1;
                        aString1 = rPar->Get( i )->GetString();
                    }
                    break;
                case SbxBOOL:
                    if ( nParams & PARAM_BOOL_1 )
                        if ( nParams & PARAM_BOOL_2 )
                            SbxBase::SetError( SbxERR_WRONG_ARGS );
                        else
                        {
                            nParams |= PARAM_BOOL_2;
                            bBool2 = rPar->Get( i )->GetBool();
                        }
                    else
                    {
                        nParams |= PARAM_BOOL_1;
                        bBool1 = rPar->Get( i )->GetBool();
                    }
                    break;
                default:
                    SbxBase::SetError( SbxERR_WRONG_ARGS );
                    break;
            }
        }
    }
    Write (nParams);
    if( nParams & PARAM_UINT16_1 )  Write( nNr1 );
    if( nParams & PARAM_UINT16_2 )  Write( nNr2 );
    if( nParams & PARAM_UINT16_3 )  Write( nNr3 );
    if( nParams & PARAM_UINT16_4 )  Write( nNr4 );
    if( nParams & PARAM_UINT32_1 )  Write( nLNr1 );
    if( nParams & PARAM_STR_1 )     Write( aString1, IsKeyString );
    if( nParams & PARAM_STR_2 )     Write( aString2, IsKeyString );
    if( nParams & PARAM_BOOL_1 )    Write( bBool1 );
    if( nParams & PARAM_BOOL_2 )    Write( bBool2 );
}

void CmdStream::GenCmdCommand( sal_uInt16 nNr, SbxArray* rPar )
{
    Write(sal_uInt16(SICommand));
    Write(nNr);
    WriteSortedParams(rPar, (nNr & M_KEY_STRING) != 0 );
}

void CmdStream::GenCmdSlot( sal_uInt16 nNr, SbxArray* rPar )
{
    Write(sal_uInt16(SISlot));
    Write(nNr);
    if (rPar)
    {
        sal_uInt16 nAnz = (rPar->Count()-1) >> 1;
        Write(nAnz);
        sal_Bool bWriteUnoSlot = rPar->Get( 1 )->GetType() == SbxSTRING;

        for (sal_uInt16 n = 1 ; n <= nAnz ; n++)
        {
            /// #59513# not necessary anymore
//          sal_uLong nUserData = rPar->Get( 2*n-1 )->GetUserData();
//          rPar->Get( 2*n-1 )->SetUserData(ID_DoNothing);  // prevents execution of the slots that are passed as parameters

            if ( bWriteUnoSlot )
                Write(rPar->Get( 2*n-1 )->GetString());
            else
                Write(rPar->Get( 2*n-1 )->GetUShort());
            switch (rPar->Get( 2*n )->GetType())
            {
                case SbxINTEGER:
                case SbxBYTE:
                case SbxUSHORT:
                case SbxINT:
                case SbxUINT:
                case SbxSINGLE:
                    if ( !bWriteUnoSlot )
                        Write( (sal_uInt16)BinUINT16 );
                    Write(rPar->Get( 2*n )->GetUShort());
                    break;
                case SbxLONG:
                case SbxULONG:
                case SbxSALINT64:
                case SbxSALUINT64:
                case SbxDOUBLE:
                    if ( !bWriteUnoSlot )
                        Write( (sal_uInt16)BinUINT32 );
                    Write(rPar->Get( 2*n )->GetULong());
                    break;
                case SbxSTRING:
                case SbxCHAR:
                    if ( !bWriteUnoSlot )
                        Write( (sal_uInt16)BinString);
                    Write((String)rPar->Get( 2*n )->GetString());    // Cast for OS/2
                    break;
                case SbxBOOL:
                    if ( !bWriteUnoSlot )
                        Write( (sal_uInt16)BinBool);
                    Write(rPar->Get( 2*n )->GetBool());
                    break;
                default:
                    SbxBase::SetError( SbxERR_WRONG_ARGS );
                    break;
            }

            /// #59513# //don't need this anymore (see prior comment?)
//          rPar->Get( 2*n-1 )->SetUserData(nUserData); // Set it back, so everything's clean.
        }
    }
    else
        Write(sal_uInt16(0));
}

void CmdStream::GenCmdUNOSlot( const String &aURL )
{
    Write( sal_uInt16(SIUnoSlot) );
/*  Write( sal_uInt16(0) );     // The SID_OPENURL is entered here.
                            // That must be wired hard in the Office and not here of course
                            // as the ID might change

    // Because the ID must also be taken for the PoolItem from the Office, it is not
    // necessary to send a PoolItem here.

    Write( sal_uInt16(0) );     // number of PoolItems

    // instead there's an extra string sent by the Office which is changed to an
    // SfxStringItem with the respective ID
    Write( aURL );          // String for the PoolItem*/

    Write( aURL );          // UNO URL
}

void CmdStream::GenCmdControl( comm_UINT32 nUId, sal_uInt16 nMethodId, SbxArray* rPar )
{
    Write(sal_uInt16(SIControl));
    Write(nUId);
    Write(nMethodId);
    WriteSortedParams(rPar, (nMethodId & M_KEY_STRING) != 0 );
}

void CmdStream::GenCmdControl( String aUId, sal_uInt16 nMethodId, SbxArray* rPar )
{
    Write(sal_uInt16(SIStringControl));
    Write(aUId);
    Write(nMethodId);
    WriteSortedParams(rPar, (nMethodId & M_KEY_STRING) != 0 );
}

void CmdStream::GenCmdFlow( sal_uInt16 nArt )
{
    Write(sal_uInt16(SIFlow));
    Write(nArt);
    Write(sal_uInt16(PARAM_NONE));              // type of the following parameters
}

void CmdStream::GenCmdFlow( sal_uInt16 nArt, comm_UINT32 nNr1 )
{
    Write(sal_uInt16(SIFlow));
    Write(nArt);
    Write(sal_uInt16(PARAM_UINT32_1));          // type of the following parameters
    Write(nNr1);
}

void CmdStream::Write( String aString, sal_Bool IsKeyString )
{
    if ( IsKeyString )
        Write( WandleKeyEventString( aString ), sal_False );
    else
        Write( aString.GetBuffer(), aString.Len() );
}

SvMemoryStream* CmdStream::GetStream()
{
    return pSammel;
}

void CmdStream::Reset( comm_UINT32 nSequence )
{
    delete pCommStream;
    delete pSammel;
    pSammel = new SvMemoryStream();
    pCommStream = new SvCommStream( pSammel );
    GenCmdFlow (F_Sequence,nSequence);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
