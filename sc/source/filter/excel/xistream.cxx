/*************************************************************************
 *
 *  $RCSfile: xistream.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: dr $ $Date: 2002-11-21 12:12:52 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifdef PCH
#include "filt_pch.hxx"
#endif

#pragma hdrstop


// ============================================================================

#ifndef SC_XISTREAM_HXX
#include "xistream.hxx"
#endif

#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif

#ifndef SC_XIROOT_HXX
#include "xiroot.hxx"
#endif


// ============================================================================

XclImpDecrypter::~XclImpDecrypter()
{
}

sal_uInt16 XclImpDecrypter::GetHash( const ByteString& rPass )
{
    sal_uInt16 nLen = static_cast< sal_uInt16 >( ::std::min< xub_StrLen >( rPass.Len(), 0xFFFF ) );
    sal_uInt16 nHash = nLen;
    if( nLen )
        nHash ^= 0xCE4B;

    for( sal_uInt16 nIndex = 0; nIndex < nLen; ++nIndex )
    {
        sal_uInt16 nChar = static_cast< sal_uInt8 >( rPass.GetChar( nIndex ) );
        sal_uInt8 nRot = static_cast< sal_uInt8 >( (nIndex + 1) % 15 );
        ::rotate_left( nChar, nRot, 15 );
        nHash ^= nChar;
    }
    return nHash;
}

void lcl_GetNextKeyBase( sal_uInt16& rnKey )
{
}

sal_uInt16 XclImpDecrypter::GetKey( const ByteString& rPass )
{
    xub_StrLen nLen = rPass.Len();
    if( !nLen ) return 0;

    sal_uInt16 nKey = 0;
    sal_uInt16 nKeyBase = 0x8000;
    sal_uInt16 nKeyEnd = 0xFFFF;
    for( xub_StrLen nIndex = 0; nIndex < nLen; ++nIndex )
    {
        sal_Char nChar = rPass.GetChar( nLen - nIndex - 1 ) & 0x7F;
        for( sal_uInt8 nBit = 0; nBit < 8; ++nBit )
        {
            ::rotate_left( nKeyBase, 1 );
            if( nKeyBase & 1 ) nKeyBase ^= 0x1020;
            if( nChar & 1 ) nKey ^= nKeyBase;
            nChar >>= 1;
            ::rotate_left( nKeyEnd, 1 );
            if( nKeyEnd & 1 ) nKeyEnd ^= 0x1020;
        }
    }
    return nKey ^ nKeyEnd;
}


// ----------------------------------------------------------------------------

XclImpBiff5Decrypter::XclImpBiff5Decrypter( const String& rPass, sal_uInt16 nKey, sal_uInt16 nHash ) :
    mbIsValid( false )
{
    ByteString aPass( rPass, RTL_TEXTENCODING_MS_1252 );    //! correct?
    xub_StrLen nLen = aPass.Len();
    if( (0 < nLen) && (nLen < 16) && (GetKey( aPass ) == nKey) && (GetHash( aPass ) == nHash) )
    {
        static const sal_uInt8 pFillChars[] =
        {
            0xBB, 0xFF, 0xFF, 0xBA,
            0xFF, 0xFF, 0xB9, 0x80,
            0x00, 0xBE, 0x0F, 0x00,
            0xBF, 0x0F, 0x00
        };
        const sal_uInt8* pFillChar = pFillChars;

        xub_StrLen nIndex;
        for( nIndex = 0; nIndex < nLen; ++nIndex )
            mpKey[ nIndex ] = aPass.GetChar( nIndex );
        for( ; nIndex < 16; ++nIndex, ++pFillChar )
            mpKey[ nIndex ] = *pFillChar;

        SVBT16 nOrigKey;
        ShortToSVBT16( nKey, nOrigKey );
        for( nIndex = 0; nIndex < 16; ++nIndex )
        {
            sal_uInt8& rnVal = mpKey[ nIndex ];
            rnVal ^= nOrigKey[ nIndex & 0x01 ];
            ::rotate_left( rnVal, 2 );
        }

        mbIsValid = true;
    }
}

bool XclImpBiff5Decrypter::IsValid() const
{
    return mbIsValid;
}

void XclImpBiff5Decrypter::SetOffset( sal_uInt16 nRecSize )
{
    mnOffset = nRecSize & 0x0F;
}

sal_uInt32 XclImpBiff5Decrypter::ReadDecrypt( SvStream& rStrm, void* pData, sal_uInt32 nBytes )
{
    DBG_ASSERT( mbIsValid, "XclImpBiff5Decrypter::ReadDecrypt - invalid decrypter" );

    sal_uInt32 nRet = 0;
    if( pData && nBytes )
    {
        sal_uInt8* pCurrKey = mpKey + ((rStrm.Tell() + mnOffset) & 0x0F);
        sal_uInt8* pKeyLast = mpKey + 0x0F;

        nRet = rStrm.Read( pData, nBytes );

        sal_uInt32 nBytesLeft = nBytes;
        sal_uInt8* pBuffer = reinterpret_cast< sal_uInt8* >( pData );
        const sal_uInt8* pBufferEnd = pBuffer + nBytes;
        for( ; pBuffer < pBufferEnd; ++pBuffer )
        {
            ::rotate_left( *pBuffer, 3 );
            *pBuffer ^= *pCurrKey;
            if( pCurrKey < pKeyLast ) ++pCurrKey; else pCurrKey = mpKey;
        }
    }
    return nRet;
}


// ----------------------------------------------------------------------------

XclImpBiff8Decrypter::XclImpBiff8Decrypter( const String& rPass )
{
    DBG_ERRORFILE( "XclImpBiff8Decrypter::XclImpBiff8Decrypter - not implemented" );
}

bool XclImpBiff8Decrypter::IsValid() const
{
    return false;
}

void XclImpBiff8Decrypter::SetOffset( sal_uInt16 nRecSize )
{
}

sal_uInt32 XclImpBiff8Decrypter::ReadDecrypt( SvStream& rStrm, void* pData, sal_uInt32 nBytes )
{
    return rStrm.Read( pData, nBytes );
}


// ============================================================================

XclImpStreamPos::XclImpStreamPos(
        sal_uInt32 nStrmPos, sal_uInt32 nNextPos,
        sal_uInt32 nCurrSize, sal_uInt32 nRecLeft, sal_uInt16 nRecSize ) :
    mnPos( nStrmPos ),
    mnNextPos( nNextPos ),
    mnCurrSize( nCurrSize ),
    mnRecLeft( nRecLeft ),
    mnRecSize( nRecSize )
{
}

void XclImpStreamPos::Set(
        sal_uInt32 nStrmPos, sal_uInt32 nNextPos,
        sal_uInt32 nCurrSize, sal_uInt32 nRecLeft, sal_uInt16 nRecSize )
{
    mnPos = nStrmPos;
    mnNextPos = nNextPos;
    mnCurrSize = nCurrSize;
    mnRecLeft = nRecLeft;
    mnRecSize = nRecSize;
}

void XclImpStreamPos::Get(
        sal_uInt32& rnStrmPos, sal_uInt32& rnNextPos,
        sal_uInt32& rnCurrSize, sal_uInt32& rnRecLeft, sal_uInt16& rnRecSize ) const
{
    rnStrmPos = mnPos;
    rnNextPos = mnNextPos;
    rnCurrSize = mnCurrSize;
    rnRecLeft = mnRecLeft;
    rnRecSize = mnRecSize;
}


// ============================================================================

XclImpStream::XclImpStream( SvStream& rInStrm, const XclImpRoot& rRoot, bool bContHandling ) :
    mrStrm( rInStrm ),
    mrRoot( rRoot ),
    maFirstRec( STREAM_SEEK_TO_BEGIN, STREAM_SEEK_TO_BEGIN, 0, 0, 0 ),
    maGlobPos( STREAM_SEEK_TO_BEGIN, STREAM_SEEK_TO_BEGIN, 0, 0, 0 ),
    mnGlobRecId( 0 ),
    mbGlobValidRec( false ),
    mbHasGlobPos( false ),
    mnNextRecPos( STREAM_SEEK_TO_BEGIN ),
    mnCurrRecSize( 0 ),
    mnComplRecSize( 0 ),
    mbHasComplRec( false ),
    mnRecId( EXC_ID_UNKNOWN ),
    mnAltContId( EXC_ID_UNKNOWN ),
    mnRecSize( 0 ),
    mnRecLeft( 0 ),
    mbCont( bContHandling ),
    mbUseDecr( false ),
    mbValidRec( false ),
    mbValid( false ),
    mbWarnings( true )
{
    mnStreamSize = mrStrm.Seek( STREAM_SEEK_TO_END );
    mrStrm.Seek( STREAM_SEEK_TO_BEGIN );
    DBG_ASSERT( mnStreamSize < STREAM_SEEK_TO_END, "XclImpStream::XclImpStream - stream error" );
}

XclImpStream::~XclImpStream()
{
}

bool XclImpStream::GetNextRecord( sal_uInt16& rnRecId, sal_uInt16& rnRecSize )
{
    mrStrm.Seek( mnNextRecPos );
    bool bRet = (mnNextRecPos < mnStreamSize);
    if( bRet )
    {
        mrStrm >> rnRecId >> rnRecSize;  // read direct
        SetDecrypterOffset( rnRecSize );
    }
    else
        rnRecId = rnRecSize = 0;
    return bRet;
}

void XclImpStream::SetupRecord()
{
    mnRecLeft = mnRecSize;
    mnCurrRecSize = mnComplRecSize = mnRecSize;
    mbHasComplRec = !mbCont;
    maFirstRec.Set( Tell(), mnNextRecPos, mnComplRecSize, mnRecLeft, mnRecSize );
}

bool XclImpStream::IsContinueId( sal_uInt16 nRecId )
{
    return (nRecId == EXC_ID_CONT) || (nRecId == mnAltContId);
}

bool XclImpStream::StartNextRecord()
{
    maPosStack.Clear();

    /*  counter to ignore zero records (id==len==0) (i.e. the application
        "Crystal Report" writes zero records between other records) */
    sal_uInt32 nZeroRecCount = 5;
    bool bIsZeroRec = false;

    do
    {
        mbValidRec = GetNextRecord( mnRecId, mnRecSize );
        bIsZeroRec = !mnRecId && !mnRecSize;
        if( bIsZeroRec ) --nZeroRecCount;
        mnNextRecPos = Tell() + mnRecSize;
    }
    while( mbValidRec && ((mbCont && IsContinueId( mnRecId )) || (bIsZeroRec && nZeroRecCount)) );

    if( bIsZeroRec )
        mbValidRec = false;
    mbValid = mbValidRec;
    mnAltContId = EXC_ID_UNKNOWN;
    SetupRecord();
    return mbValidRec;
}

void XclImpStream::InitializeRecord( bool bContHandling, sal_uInt16 nAltContId )
{
    if( mbValidRec )
    {
        maPosStack.Clear();
        RestorePosition( maFirstRec );
        mnCurrRecSize = mnComplRecSize = mnRecSize;
        mbHasComplRec = !bContHandling;
        mbCont = bContHandling;
        mnAltContId = nAltContId;
    }
}

void XclImpStream::EnableDecryption( XclImpDecrypter* pDecrypter )
{
    mpDecrypter.reset( pDecrypter );
    SetDecrypterOffset( mnRecSize );
    UseDecryption( true );
}

void XclImpStream::UseDecryption( bool bUse )
{
    mbUseDecr = (mpDecrypter.get() && mpDecrypter->IsValid()) ? bUse : false;
}

bool XclImpStream::GetContinue()
{
    bool bRet = mbCont || IsContinueId( mnRecId );
    if( bRet )
    {
        sal_uInt16 nNewNum;
        bRet = GetNextRecord( nNewNum, mnRecSize ) && IsContinueId( nNewNum );
    }
    if( bRet )
    {
        mnRecLeft = mnRecSize;
        mnNextRecPos = Tell() + mnRecSize;
        mnCurrRecSize += mnRecSize;
    }
    return bRet;
}

bool XclImpStream::CheckDataLeft( sal_uInt32 nBytes )
{
    bool bNewValid = mbValid && ((mnRecLeft >= nBytes) ? true : (mnRecLeft ? false : GetContinue()));
    DBG_ASSERT( !mbWarnings || bNewValid || !mbValid, "XclImpStream - record overread" ); // one assertion per record
    return (mbValid = bNewValid);   // really assignment
}

void XclImpStream::StartContinue()
{
    bool bNewValid = (mbValid && !mnRecLeft) ? GetContinue() : false;
    DBG_ASSERT( !mbWarnings || bNewValid || !mbValid, "XclImpStream - record overread" ); // one assertion per record
    mbValid = bNewValid;
}


// ----------------------------------------------------------------------------

void XclImpStream::PushPosition()
{
    if( mbValid )
        maPosStack.Push( new XclImpStreamPos(
            Tell(), mnNextRecPos, mnCurrRecSize, mnRecLeft, mnRecSize ) );
}

void XclImpStream::RestorePosition( const XclImpStreamPos& rPos )
{
    sal_uInt32 nPos;
    rPos.Get( nPos, mnNextRecPos, mnCurrRecSize, mnRecLeft, mnRecSize );
    mrStrm.Seek( nPos );
    mbValid = true;
}

void XclImpStream::PopPosition()
{
    XclImpStreamPos* pPos = maPosStack.Pop();
    DBG_ASSERT( pPos, "XclImpStream::PopPosition - stack empty" );
    if( pPos )
    {
        RestorePosition( *pPos );
        delete pPos;
    }
}

void XclImpStream::RejectPosition()
{
    DBG_ASSERT( !maPosStack.Empty(), "XclImpStream::RejectPosition - stack empty" );
    delete maPosStack.Pop();
}

void XclImpStream::StoreGlobalPosition()
{
    maGlobPos.Set( Tell(), mnNextRecPos, mnCurrRecSize, mnRecLeft, mnRecSize );
    mnGlobRecId = mnRecId;
    mbGlobValidRec = mbValidRec;
    mbHasGlobPos = true;
}

void XclImpStream::SeekGlobalPosition()
{
    DBG_ASSERT( mbHasGlobPos, "XclImpStream::SeekGlobalPosition - no position stored" );
    if( mbHasGlobPos )
    {
        RestorePosition( maGlobPos );
        mnRecId = mnGlobRecId;
        mnComplRecSize = mnCurrRecSize;
        mbHasComplRec = !mbCont;
        mbValidRec = mbValid = mbGlobValidRec;
        SetDecrypterOffset( mnRecSize );
    }
}

sal_uInt32 XclImpStream::GetRecSize()
{
    if( IsValid() && !mbHasComplRec )
    {
        PushPosition();
        while( GetContinue() );     // GetContinue() adds up mnCurrRecSize
        mnComplRecSize = mnCurrRecSize;
        mbHasComplRec = true;
        PopPosition();
    }
    return mnComplRecSize;
}


// ----------------------------------------------------------------------------

void XclImpStream::ReadAtom( sal_Int8& rnValue )
{
    if( mbUseDecr )
        mpDecrypter->ReadDecrypt( mrStrm, &rnValue, 1 );
    else
        mrStrm >> rnValue;
    --mnRecLeft;
}

void XclImpStream::ReadAtom( sal_uInt8& rnValue )
{
    if( mbUseDecr )
        mpDecrypter->ReadDecrypt( mrStrm, &rnValue, 1 );
    else
        mrStrm >> rnValue;
    --mnRecLeft;
}

void XclImpStream::ReadAtom( sal_Int16& rnValue )
{
    if( mbUseDecr )
    {
        SVBT16 pBuffer;
        mpDecrypter->ReadDecrypt( mrStrm, pBuffer, 2 );
        rnValue = static_cast< sal_Int16 >( SVBT16ToShort( pBuffer ) );
    }
    else
        mrStrm >> rnValue;
    mnRecLeft -= 2;
}

void XclImpStream::ReadAtom( sal_uInt16& rnValue )
{
    if( mbUseDecr )
    {
        SVBT16 pBuffer;
        mpDecrypter->ReadDecrypt( mrStrm, pBuffer, 2 );
        rnValue = SVBT16ToShort( pBuffer );
    }
    else
        mrStrm >> rnValue;
    mnRecLeft -= 2;
}

void XclImpStream::ReadAtom( sal_Int32& rnValue )
{
    if( mbUseDecr )
    {
        SVBT32 pBuffer;
        mpDecrypter->ReadDecrypt( mrStrm, pBuffer, 4 );
        rnValue = static_cast< sal_Int32 >( SVBT32ToLong( pBuffer ) );
    }
    else
        mrStrm >> rnValue;
    mnRecLeft -= 4;
}

void XclImpStream::ReadAtom( sal_uInt32& rnValue )
{
    if( mbUseDecr )
    {
        SVBT32 pBuffer;
        mpDecrypter->ReadDecrypt( mrStrm, pBuffer, 4 );
        rnValue = SVBT32ToLong( pBuffer );
    }
    else
        mrStrm >> rnValue;
    mnRecLeft -= 4;
}

void XclImpStream::ReadAtom( float& rfValue )
{
    if( mbUseDecr )
    {
        SVBT32 pBuffer;
        mpDecrypter->ReadDecrypt( mrStrm, pBuffer, 4 );
        sal_uInt32 nValue = SVBT32ToLong( pBuffer );
        memcpy( &rfValue, &nValue, 4 );
    }
    else
        mrStrm >> rfValue;
    mnRecLeft -= 4;
}

void XclImpStream::ReadAtom( double& rfValue )
{
    if( mbUseDecr )
    {
        SVBT64 pBuffer;
        mpDecrypter->ReadDecrypt( mrStrm, pBuffer, 8 );
        rfValue = SVBT64ToDouble( pBuffer );
    }
    else
        mrStrm >> rfValue;
    mnRecLeft -= 8;
}

sal_uInt32 XclImpStream::ReadData( void* pData, sal_uInt32 nBytes )
{
    sal_uInt32 nRet = mbUseDecr ?
        mpDecrypter->ReadDecrypt( mrStrm, pData, nBytes ) : mrStrm.Read( pData, nBytes );
    mnRecLeft -= nRet;
    return nRet;
}


// ----------------------------------------------------------------------------

sal_Int8 XclImpStream::ReadInt8()
{
    sal_Int8 nValue;
    operator>>( nValue );
    return nValue;
}

sal_uInt8 XclImpStream::ReaduInt8()
{
    sal_uInt8 nValue;
    operator>>( nValue );
    return nValue;
}

sal_Int16 XclImpStream::ReadInt16()
{
    sal_Int16 nValue;
    operator>>( nValue );
    return nValue;
}

sal_uInt16 XclImpStream::ReaduInt16()
{
    sal_uInt16 nValue;
    operator>>( nValue );
    return nValue;
}

sal_Int32 XclImpStream::ReadInt32()
{
    sal_Int32 nValue;
    operator>>( nValue );
    return nValue;
}

sal_uInt32 XclImpStream::ReaduInt32()
{
    sal_uInt32 nValue;
    operator>>( nValue );
    return nValue;
}

float XclImpStream::ReadFloat()
{
    float fValue;
    operator>>( fValue );
    return fValue;
}

double XclImpStream::ReadDouble()
{
    double fValue;
    operator>>( fValue );
    return fValue;
}

sal_uInt32 XclImpStream::Read( void* pData, sal_uInt32 nBytes )
{
    sal_uInt32 nRet = 0;
    if( IsValid() && pData && nBytes )
    {
        sal_uInt8* pBuffer = reinterpret_cast< sal_uInt8* >( pData );
        sal_uInt32 nBytesLeft = nBytes;

        while( IsValid() && nBytesLeft )
        {
            sal_uInt32 nReadSize = ::std::min( nBytesLeft, mnRecLeft );
            sal_uInt32 nReadRet = ReadData( pBuffer, nReadSize );
            nRet += nReadRet;
            mbValid = (nReadSize == nReadRet);
            DBG_ASSERT( !mbWarnings || IsValid(), "XclImpStream::Read - stream read error" );
            pBuffer += nReadRet;
            nBytesLeft -= nReadRet;
            if( IsValid() && nBytesLeft )
                StartContinue();
        }
    }
    return nRet;
}

sal_uInt32 XclImpStream::CopyToStream( SvStream& rOutStrm, sal_uInt32 nBytes )
{
    sal_uInt32 nRet = 0;
    if( IsValid() && nBytes )
    {
        const sal_uInt32 nMaxBuffer = 0x1000;
        sal_uInt8* pBuffer = new sal_uInt8[ ::std::min( nBytes, nMaxBuffer ) ];
        sal_uInt32 nBytesLeft = nBytes;

        while( IsValid() && nBytesLeft )
        {
            sal_uInt32 nReadSize = ::std::min( nBytesLeft, nMaxBuffer );
            nRet += Read( pBuffer, nReadSize );
            rOutStrm.Write( pBuffer, nReadSize );
            nBytesLeft -= nReadSize;
        }

        delete[] pBuffer;
    }
    return nRet;
}

sal_uInt32 XclImpStream::CopyRecordToStream( SvStream& rOutStrm )
{
    sal_uInt32 nRet = 0;
    if( mbValidRec )
    {
        PushPosition();
        RestorePosition( maFirstRec );
        nRet = CopyToStream( rOutStrm, GetRecSize() );
        PopPosition();
    }
    return nRet;
}

void XclImpStream::Seek( sal_uInt32 nPos )
{
    if( mbValidRec )
    {
        sal_uInt32 nCurrPos = GetRecPos();
        if( nPos > nCurrPos )       // forward
            Ignore( nPos - nCurrPos );
        else if( nPos < nCurrPos )  // backward
        {
            RestorePosition( maFirstRec );
            Ignore( nPos );
        }
    }
}

void XclImpStream::Ignore( sal_uInt32 nBytes )
{
    sal_uInt32 nBytesLeft = nBytes;
    while( IsValid() && nBytesLeft )
    {
        sal_uInt32 nReadSize = ::std::min( nBytesLeft, mnRecLeft );
        mrStrm.SeekRel( static_cast< sal_Int32 >( nReadSize ) );
        mnRecLeft -= nReadSize;
        nBytesLeft -= nReadSize;
        if( nBytesLeft )
            StartContinue();
    }
}


// ----------------------------------------------------------------------------

void XclImpStream::StartStringContinue( bool& rb16Bit )
{
    DBG_ASSERT( !mbWarnings || !mnRecLeft, "XclImpStream::StartStringContinue - unexpected garbage" );

    if( mbCont && GetRecLeft() )
        StartContinue();
    else if( mnRecId == EXC_ID_CONT )   // start next CONTINUE for TXO import
    {
        sal_uInt16 nNewNum, nNewSize;
        mbValidRec = GetNextRecord( nNewNum, nNewSize ) && (nNewNum || nNewSize);
        mbValid = mbValidRec && (nNewNum == EXC_ID_CONT);
        if( mbValid )
        {
            mnRecSize = nNewSize;
            mnNextRecPos = Tell() + nNewSize;
            SetupRecord();
        }
    }
    else
        mbValid = false;

    if( mbValid )
        rb16Bit = HasFlag( ReaduInt8(), EXC_STRF_16BIT );
}

sal_uInt32 XclImpStream::ReadUniStringExtHeader(
        bool& rb16Bit, bool& rbRich, bool& rbFareast,
        sal_uInt16& rnCrun, sal_uInt32& rnExtInf, sal_uInt8 nFlags )
{
    DBG_ASSERT( !HasFlag( nFlags, EXC_STRF_UNKNOWN ), "XclImpStream::ReadUniStringExt - unknown flags" );
    rb16Bit = HasFlag( nFlags, EXC_STRF_16BIT );
    rbRich = HasFlag( nFlags, EXC_STRF_RICH );
    rbFareast = HasFlag( nFlags, EXC_STRF_FAREAST );
    rnCrun = rbRich ? ReaduInt16() : 0;
    rnExtInf = rbFareast ? ReaduInt32() : 0;
    return rnExtInf + 4UL * rnCrun;
}

sal_uInt32 XclImpStream::ReadUniStringExtHeader( bool& rb16Bit, sal_uInt8 nFlags )
{
    bool bRich, bFareast;
    sal_uInt16 nCrun;
    sal_uInt32 nExtInf;
    return ReadUniStringExtHeader( rb16Bit, bRich, bFareast, nCrun, nExtInf, nFlags );
}

void XclImpStream::AppendRawUniString( String& rString, sal_uInt16 nChars, bool b16Bit )
{
    sal_Char* pBuffer8 = NULL;
    sal_Unicode* pBuffer16 = NULL;
    sal_uInt32 nCharsLeft = nChars;
    sal_uInt32 nReadSize, nReadRet;

    while( IsValid() && nCharsLeft )
    {
        if( b16Bit )
        {
            if( !pBuffer16 )
                pBuffer16 = new sal_Unicode[ nCharsLeft + 1 ];
            nReadSize = ::std::min( nCharsLeft, mnRecLeft / 2 );
            DBG_ASSERT( !mbWarnings || (nReadSize <= nCharsLeft) || !(mnRecLeft & 0x1),
                "XclImpStream::ReadRawUniString - missing a byte" );
            sal_Unicode* pUniChar = pBuffer16;
            sal_Unicode* pEndChar = pBuffer16 + nReadSize;
            for( ; IsValid() && (pUniChar < pEndChar); ++pUniChar )
                operator>>( *pUniChar );
            *pUniChar = 0;
            if( *pBuffer16 )
                rString.Append( pBuffer16 );
        }
        else
        {
            if( !pBuffer8 )
                pBuffer8 = new sal_Char[ nCharsLeft + 1 ];
            nReadSize = ::std::min( nCharsLeft, mnRecLeft );
            nReadRet = Read( pBuffer8, nReadSize );
            pBuffer8[ nReadRet ] = 0;
            if( *pBuffer8 )
                rString += String( pBuffer8, mrRoot.GetCharSet() );
        }

        nCharsLeft -= nReadSize;
        if( nCharsLeft )
            StartStringContinue( b16Bit );
    }

    delete[] pBuffer8;
    delete[] pBuffer16;
}

void XclImpStream::IgnoreRawUniString( sal_uInt16 nChars, bool b16Bit )
{
    sal_uInt32 nCharsLeft = nChars;
    sal_uInt32 nReadSize;

    while( IsValid() && nCharsLeft )
    {
        if( b16Bit )
        {
            nReadSize = ::std::min( nCharsLeft, mnRecLeft / 2 );
            DBG_ASSERT( !mbWarnings || (nReadSize <= nCharsLeft) || !(mnRecLeft & 0x1),
                "XclImpStream::IgnoreRawUniString - missing a byte" );
            Ignore( nReadSize * 2 );
        }
        else
        {
            nReadSize = ::std::min( nCharsLeft, mnRecLeft );
            Ignore( nReadSize );
        }

        nCharsLeft -= nReadSize;
        if( nCharsLeft )
            StartStringContinue( b16Bit );
    }
}

void XclImpStream::AppendUniString( String& rString, sal_uInt16 nChars, sal_uInt8 nFlags )
{
    bool b16Bit;
    sal_uInt32 nExtSize = ReadUniStringExtHeader( b16Bit, nFlags );
    AppendRawUniString( rString, nChars, b16Bit );
    SkipUniStringExtData( nExtSize );
}

String XclImpStream::ReadRawUniString( sal_uInt16 nChars, bool b16Bit )
{
    String aRet;
    AppendRawUniString( aRet, nChars, b16Bit );
    return aRet;
}

String XclImpStream::ReadUniString( sal_uInt16 nChars, sal_uInt8 nFlags )
{
    String aRet;
    AppendUniString( aRet, nChars, nFlags );
    return aRet;
}

String XclImpStream::ReadUniString( sal_uInt16 nChars )
{
    String aRet;
    AppendUniString( aRet, nChars );
    return aRet;
}

String XclImpStream::ReadUniString()
{
    String aRet;
    AppendUniString( aRet );
    return aRet;
}

void XclImpStream::IgnoreUniString( sal_uInt16 nChars, sal_uInt8 nFlags )
{
    bool b16Bit;
    sal_uInt32 nExtSize = ReadUniStringExtHeader( b16Bit, nFlags );
    IgnoreRawUniString( nChars, b16Bit );
    SkipUniStringExtData( nExtSize );
}

String XclImpStream::ReadRawByteString( sal_uInt16 nChars )
{
    String aRet;
    AppendRawByteString( aRet, nChars );
    return aRet;
}

String XclImpStream::ReadByteString( bool b16BitLen )
{
    String aRet;
    AppendByteString( aRet, b16BitLen );
    return aRet;
}


// ----------------------------------------------------------------------------

sal_uInt32 XclImpStream::Tell() const
{
    return mrStrm.Tell();
}


// ============================================================================

