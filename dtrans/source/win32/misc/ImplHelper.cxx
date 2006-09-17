/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ImplHelper.cxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 17:02:53 $
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
#include "precompiled_dtrans.hxx"


//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef _IMPLHELPER_HXX_
#include "ImplHelper.hxx"
#endif

#ifndef _RTL_TENCINFO_H
#include <rtl/tencinfo.h>
#endif

#ifndef _RTL_MEMORY_H_
#include <rtl/memory.h>
#endif

#include <memory>
#if defined _MSC_VER
#pragma warning(push,1)
#endif
#include <windows.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif

//------------------------------------------------------------------------
// defines
//------------------------------------------------------------------------

#define FORMATETC_EXACT_MATCH    1
#define FORMATETC_PARTIAL_MATCH -1
#define FORMATETC_NO_MATCH       0

//------------------------------------------------------------------------
// namespace directives
//------------------------------------------------------------------------

using ::rtl::OUString;
using ::rtl::OString;

//------------------------------------------------------------------------
// returns a windows codepage appropriate to the
// given mime charset parameter value
//------------------------------------------------------------------------

sal_uInt32 SAL_CALL getWinCPFromMimeCharset( const OUString& charset )
{
    sal_uInt32 winCP = GetACP( );

    if ( charset.getLength( ) )
    {
        OString osCharset(
            charset.getStr( ), charset.getLength( ), RTL_TEXTENCODING_ASCII_US );

        rtl_TextEncoding txtEnc =
            rtl_getTextEncodingFromMimeCharset( osCharset.getStr( ) );

        sal_uInt32 winChrs = rtl_getBestWindowsCharsetFromTextEncoding( txtEnc );

        CHARSETINFO chrsInf;
        sal_Bool bRet = TranslateCharsetInfo( (DWORD*)winChrs, &chrsInf, TCI_SRCCHARSET ) ?
                        sal_True : sal_False;

        // if one of the above functions fails
        // we will return the current ANSI codepage
        // of this thread
        if ( bRet )
            winCP = chrsInf.ciACP;
    }

    return winCP;
}

//--------------------------------------------------
// returns a windows codepage appropriate to the
// given locale and locale type
//--------------------------------------------------

OUString SAL_CALL getWinCPFromLocaleId( LCID lcid, LCTYPE lctype )
{
    OSL_ASSERT( IsValidLocale( lcid, LCID_SUPPORTED ) );

    // we set an default value
    OUString winCP;

    // set an default value
    sal_Unicode wcstr[10];

    if ( LOCALE_IDEFAULTCODEPAGE == lctype )
    {
        _itow( GetOEMCP( ), wcstr, 10 );
        winCP = OUString( wcstr, wcslen( wcstr ) );
    }
    else if ( LOCALE_IDEFAULTANSICODEPAGE == lctype )
    {
        _itow( GetACP( ), wcstr, 10 );
        winCP = OUString( wcstr, wcslen( wcstr ) );
    }
    else
        OSL_ASSERT( sal_False );

    // we use the GetLocaleInfoA because don't want to provide
    // a unicode wrapper function for Win9x in sal/systools
    char buff[6];
    sal_Int32 nResult = GetLocaleInfoA(
        lcid, lctype | LOCALE_USE_CP_ACP, buff, sizeof( buff ) );

    OSL_ASSERT( nResult );

    if ( nResult )
    {
        sal_Int32 len = MultiByteToWideChar(
            CP_ACP, 0, buff, -1, NULL, 0 );

        OSL_ASSERT( len > 0 );

        std::auto_ptr< sal_Unicode > lpwchBuff( new sal_Unicode[len] );

        if ( NULL != lpwchBuff.get( ) )
        {
            len = MultiByteToWideChar(
                CP_ACP, 0, buff, -1, lpwchBuff.get( ), len );

            winCP = OUString( lpwchBuff.get( ), (len - 1) );
        }
    }

    return winCP;
}

//--------------------------------------------------
// returns a mime charset parameter value appropriate
// to the given codepage, optional a prefix can be
// given, e.g. "windows-" or "cp"
//--------------------------------------------------

OUString SAL_CALL getMimeCharsetFromWinCP( sal_uInt32 cp, const OUString& aPrefix )
{
    return aPrefix + cptostr( cp );
}

//--------------------------------------------------
// returns a mime charset parameter value appropriate
// to the given locale id and locale type, optional a
// prefix can be given, e.g. "windows-" or "cp"
//--------------------------------------------------

OUString SAL_CALL getMimeCharsetFromLocaleId( LCID lcid, LCTYPE lctype, const OUString& aPrefix  )
{
    OUString charset = getWinCPFromLocaleId( lcid, lctype );
    return aPrefix + charset;
}

//------------------------------------------------------------------------
// IsOEMCP
//------------------------------------------------------------------------

sal_Bool SAL_CALL IsOEMCP( sal_uInt32 codepage )
{
    OSL_ASSERT( IsValidCodePage( codepage ) );

    sal_uInt32 arrOEMCP[] = { 437, 708, 709, 710, 720, 737,
                              775, 850, 852, 855, 857, 860,
                              861, 862, 863, 864, 865, 866,
                              869, 874, 932, 936, 949, 950, 1361 };

    for ( sal_Int8 i = 0; i < ( sizeof( arrOEMCP )/sizeof( sal_uInt32 ) ); ++i )
        if ( arrOEMCP[i] == codepage )
            return sal_True;

    return sal_False;
}

//------------------------------------------------------------------------
// converts a codepage into its string representation
//------------------------------------------------------------------------

OUString SAL_CALL cptostr( sal_uInt32 codepage )
{
    OSL_ASSERT( IsValidCodePage( codepage ) );

    sal_Unicode cpStr[6];
    _itow( codepage, cpStr, 10 );
    return OUString( cpStr, wcslen( cpStr ) );
}

//-------------------------------------------------------------------------
// OleStdDeleteTargetDevice()
//
// Purpose:
//
// Parameters:
//
// Return Value:
//    SCODE  -  S_OK if successful
//-------------------------------------------------------------------------

void SAL_CALL DeleteTargetDevice( DVTARGETDEVICE* ptd )
{
    __try
    {
        CoTaskMemFree( ptd );
    }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {
        OSL_ENSURE( sal_False, "Error DeleteTargetDevice" );
    }
}



//-------------------------------------------------------------------------
// OleStdCopyTargetDevice()
//
// Purpose:
//  duplicate a TARGETDEVICE struct. this function allocates memory for
//  the copy. the caller MUST free the allocated copy when done with it
//  using the standard allocator returned from CoGetMalloc.
//  (OleStdFree can be used to free the copy).
//
// Parameters:
//  ptdSrc      pointer to source TARGETDEVICE
//
// Return Value:
//    pointer to allocated copy of ptdSrc
//    if ptdSrc==NULL then retuns NULL is returned.
//    if ptdSrc!=NULL and memory allocation fails, then NULL is returned
//-------------------------------------------------------------------------

DVTARGETDEVICE* SAL_CALL CopyTargetDevice( DVTARGETDEVICE* ptdSrc )
{
    DVTARGETDEVICE* ptdDest = NULL;

    __try
    {
        if ( NULL != ptdSrc )
        {
            ptdDest = static_cast< DVTARGETDEVICE* >( CoTaskMemAlloc( ptdSrc->tdSize ) );
            rtl_copyMemory( ptdDest, ptdSrc, static_cast< size_t >( ptdSrc->tdSize ) );
        }
    }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {
    }

    return ptdDest;
}


//-------------------------------------------------------------------------
// OleStdCopyFormatEtc()
//
// Purpose:
//  Copies the contents of a FORMATETC structure. this function takes
//  special care to copy correctly copying the pointer to the TARGETDEVICE
//  contained within the source FORMATETC structure.
//  if the source FORMATETC has a non-NULL TARGETDEVICE, then a copy
//  of the TARGETDEVICE will be allocated for the destination of the
//  FORMATETC (petcDest).
//
//  NOTE: the caller MUST free the allocated copy of the TARGETDEVICE
//  within the destination FORMATETC when done with it
//  using the standard allocator returned from CoGetMalloc.
//  (OleStdFree can be used to free the copy).
//
// Parameters:
//  petcDest      pointer to destination FORMATETC
//  petcSrc       pointer to source FORMATETC
//
// Return Value:
//  returns TRUE if copy was successful;
//  retuns FALSE if not successful, e.g. one or both of the pointers
//  were invalid or the pointers were equal
//-------------------------------------------------------------------------

sal_Bool SAL_CALL CopyFormatEtc( LPFORMATETC petcDest, LPFORMATETC petcSrc )
{
    sal_Bool bRet = sal_False;

    __try
    {
        if ( petcDest == petcSrc )
            __leave;

        petcDest->cfFormat = petcSrc->cfFormat;

        petcDest->ptd      = NULL;
        if ( NULL != petcSrc->ptd )
            petcDest->ptd  = CopyTargetDevice(petcSrc->ptd);

        petcDest->dwAspect = petcSrc->dwAspect;
        petcDest->lindex   = petcSrc->lindex;
        petcDest->tymed    = petcSrc->tymed;

        bRet = sal_True;
    }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {
        OSL_ENSURE( sal_False, "Error CopyFormatEtc" );
    }

    return bRet;
}

//-------------------------------------------------------------------------
// returns:
//  1 for exact match,
//  0 for no match,
// -1 for partial match (which is defined to mean the left is a subset
//    of the right: fewer aspects, null target device, fewer medium).
//-------------------------------------------------------------------------

sal_Int32 SAL_CALL CompareFormatEtc( const FORMATETC* pFetcLhs, const FORMATETC* pFetcRhs )
{
    sal_Int32 nMatch = FORMATETC_EXACT_MATCH;

    __try
    {
        if ( pFetcLhs == pFetcRhs )
            __leave;

        if ( ( pFetcLhs->cfFormat != pFetcRhs->cfFormat ) ||
             ( pFetcLhs->lindex   != pFetcRhs->lindex ) ||
             !CompareTargetDevice( pFetcLhs->ptd, pFetcRhs->ptd ) )
        {
            nMatch = FORMATETC_NO_MATCH;
            __leave;
        }

        if ( pFetcLhs->dwAspect == pFetcRhs->dwAspect )
            // same aspects; equal
            ;
        else if ( ( pFetcLhs->dwAspect & ~pFetcRhs->dwAspect ) != 0 )
        {
            // left not subset of aspects of right; not equal
            nMatch = FORMATETC_NO_MATCH;
            __leave;
        }
        else
            // left subset of right
            nMatch = FORMATETC_PARTIAL_MATCH;

        if ( pFetcLhs->tymed == pFetcRhs->tymed )
            // same medium flags; equal
            ;
        else if ( ( pFetcLhs->tymed & ~pFetcRhs->tymed ) != 0 )
        {
            // left not subset of medium flags of right; not equal
            nMatch = FORMATETC_NO_MATCH;
            __leave;
        }
        else
            // left subset of right
            nMatch = FORMATETC_PARTIAL_MATCH;
    }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {
        OSL_ENSURE( sal_False, "Error CompareFormatEtc" );
        nMatch = FORMATETC_NO_MATCH;
    }

    return nMatch;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

sal_Bool SAL_CALL CompareTargetDevice( DVTARGETDEVICE* ptdLeft, DVTARGETDEVICE* ptdRight )
{
    sal_Bool bRet = sal_False;

    __try
    {
        if ( ptdLeft == ptdRight )
        {
            // same address of td; must be same (handles NULL case)
            bRet = sal_True;
            __leave;
        }

        // one ot the two is NULL
        if ( ( NULL == ptdRight ) || ( NULL == ptdLeft ) )
            __leave;

        if ( ptdLeft->tdSize != ptdRight->tdSize )
            __leave;

        if ( rtl_compareMemory( ptdLeft, ptdRight, ptdLeft->tdSize ) == 0 )
            bRet = sal_True;
    }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {
        OSL_ENSURE( sal_False, "Error CompareTargetDevice" );
        bRet = sal_False;
    }

    return bRet;
}
