/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: txencbox.cxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 17:51:44 $
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
#include "precompiled_svx.hxx"

#include "txencbox.hxx"
#include "txenctab.hxx"
#include <svx/dialogs.hrc>

#ifndef SVX_DBCHARSETHELPER_HXX
#include "dbcharsethelper.hxx"
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _RTL_TENCINFO_H
#include <rtl/tencinfo.h>
#endif
#ifndef _RTL_LOCALE_H_
#include <rtl/locale.h>
#endif
#ifndef _OSL_NLSUPPORT_H_
#include <osl/nlsupport.h>
#endif

//========================================================================
//  class SvxTextEncodingBox
//========================================================================

SvxTextEncodingBox::SvxTextEncodingBox( Window* pParent, const ResId& rResId )
    :
    ListBox( pParent, rResId )
{
    m_pEncTable = new SvxTextEncodingTable;
}

//------------------------------------------------------------------------

SvxTextEncodingBox::~SvxTextEncodingBox()
{
    delete m_pEncTable;
}

//------------------------------------------------------------------------

USHORT SvxTextEncodingBox::EncodingToPos_Impl( rtl_TextEncoding nEnc ) const
{
    USHORT nCount = GetEntryCount();
    for ( USHORT i=0; i<nCount; i++ )
    {
        if ( nEnc == rtl_TextEncoding( (ULONG)GetEntryData(i) ) )
            return i;
    }
    return LISTBOX_ENTRY_NOTFOUND;
}

//------------------------------------------------------------------------

void SvxTextEncodingBox::FillFromTextEncodingTable(
        sal_Bool bExcludeImportSubsets, sal_uInt32 nExcludeInfoFlags,
        sal_uInt32 nButIncludeInfoFlags )
{
    rtl_TextEncodingInfo aInfo;
    aInfo.StructSize = sizeof(rtl_TextEncodingInfo);
    sal_uInt32 nCount = m_pEncTable->Count();
    for ( sal_uInt32 j=0; j<nCount; j++ )
    {
        BOOL bInsert = TRUE;
        rtl_TextEncoding nEnc = rtl_TextEncoding( m_pEncTable->GetValue( j ) );
        if ( nExcludeInfoFlags )
        {
            if ( !rtl_getTextEncodingInfo( nEnc, &aInfo ) )
                bInsert = FALSE;
            else
            {
                if ( (aInfo.Flags & nExcludeInfoFlags) == 0 )
                {
                    if ( (nExcludeInfoFlags & RTL_TEXTENCODING_INFO_UNICODE) &&
                            ((nEnc == RTL_TEXTENCODING_UCS2) ||
                            nEnc == RTL_TEXTENCODING_UCS4) )
                        bInsert = FALSE;    // InfoFlags don't work for Unicode :-(
                }
                else if ( (aInfo.Flags & nButIncludeInfoFlags) == 0 )
                    bInsert = FALSE;
            }
        }
        if ( bInsert )
        {
            if ( bExcludeImportSubsets )
            {
                switch ( nEnc )
                {
                    // subsets of RTL_TEXTENCODING_GB_18030
                    case RTL_TEXTENCODING_GB_2312 :
                    case RTL_TEXTENCODING_GBK :
                    case RTL_TEXTENCODING_MS_936 :
                        bInsert = FALSE;
                    break;
                }
            }
            if ( bInsert )
                InsertTextEncoding( nEnc, m_pEncTable->GetString( j ) );
        }
    }
}

//------------------------------------------------------------------------

void SvxTextEncodingBox::FillFromDbTextEncodingMap(
        sal_Bool bExcludeImportSubsets, sal_uInt32 nExcludeInfoFlags,
        sal_uInt32 nButIncludeInfoFlags )
{
    rtl_TextEncodingInfo aInfo;
    aInfo.StructSize = sizeof(rtl_TextEncodingInfo);
    svxform::ODataAccessCharsetHelper aCSH;
    ::std::vector< rtl_TextEncoding > aEncs;
    sal_Int32 nCount = aCSH.getSupportedTextEncodings( aEncs );
    for ( USHORT j=0; j<nCount; j++ )
    {
        BOOL bInsert = TRUE;
        rtl_TextEncoding nEnc = rtl_TextEncoding( aEncs[j] );
        if ( nExcludeInfoFlags )
        {
            if ( !rtl_getTextEncodingInfo( nEnc, &aInfo ) )
                bInsert = FALSE;
            else
            {
                if ( (aInfo.Flags & nExcludeInfoFlags) == 0 )
                {
                    if ( (nExcludeInfoFlags & RTL_TEXTENCODING_INFO_UNICODE) &&
                            ((nEnc == RTL_TEXTENCODING_UCS2) ||
                            nEnc == RTL_TEXTENCODING_UCS4) )
                        bInsert = FALSE;    // InfoFlags don't work for Unicode :-(
                }
                else if ( (aInfo.Flags & nButIncludeInfoFlags) == 0 )
                    bInsert = FALSE;
            }
        }
        if ( bInsert )
        {
            if ( bExcludeImportSubsets )
            {
                switch ( nEnc )
                {
                    // subsets of RTL_TEXTENCODING_GB_18030
                    case RTL_TEXTENCODING_GB_2312 :
                    case RTL_TEXTENCODING_GBK :
                    case RTL_TEXTENCODING_MS_936 :
                        bInsert = FALSE;
                    break;
                }
            }
            // CharsetMap offers a RTL_TEXTENCODING_DONTKNOW for internal use,
            // makes no sense here and would result in an empty string as list
            // entry.
            if ( bInsert && nEnc != RTL_TEXTENCODING_DONTKNOW )
                InsertTextEncoding( nEnc );
        }
    }
}

//------------------------------------------------------------------------

// static
rtl_TextEncoding SvxTextEncodingBox::GetBestMimeEncoding()
{
    const sal_Char* pCharSet = rtl_getBestMimeCharsetFromTextEncoding(
            gsl_getSystemTextEncoding() );
    if ( !pCharSet )
    {
        // If the system locale is unknown to us, e.g. LC_ALL=xx, match the UI
        // language if possible.
        ::com::sun::star::lang::Locale aLocale(
                Application::GetSettings().GetUILocale() );
        rtl_Locale * pLocale = rtl_locale_register( aLocale.Language.getStr(),
                aLocale.Country.getStr(), aLocale.Variant.getStr() );
        rtl_TextEncoding nEnc = osl_getTextEncodingFromLocale( pLocale );
        pCharSet = rtl_getBestMimeCharsetFromTextEncoding( nEnc );
    }
    rtl_TextEncoding nRet;
    if ( pCharSet )
        nRet = rtl_getTextEncodingFromMimeCharset( pCharSet );
    else
        nRet = RTL_TEXTENCODING_UTF8;
    return nRet;
}

//------------------------------------------------------------------------

void SvxTextEncodingBox::FillWithMimeAndSelectBest()
{
    FillFromTextEncodingTable( sal_False, 0xffffffff, RTL_TEXTENCODING_INFO_MIME );
    rtl_TextEncoding nEnc = GetBestMimeEncoding();
    SelectTextEncoding( nEnc );
}

//------------------------------------------------------------------------

void SvxTextEncodingBox::InsertTextEncoding( const rtl_TextEncoding nEnc,
            const String& rEntry, USHORT nPos )
{
    USHORT nAt = InsertEntry( rEntry, nPos );
    SetEntryData( nAt, (void*)(ULONG)nEnc );
}

//------------------------------------------------------------------------

void SvxTextEncodingBox::InsertTextEncoding( const rtl_TextEncoding nEnc, USHORT nPos )
{
    const String& rEntry = m_pEncTable->GetTextString( nEnc );
    if ( rEntry.Len() )
        InsertTextEncoding( nEnc, rEntry, nPos );
    else
    {
#ifdef DBG_UTIL
        ByteString aMsg( "SvxTextEncodingBox::InsertTextEncoding: no resource string for text encoding: " );
        aMsg += ByteString::CreateFromInt32( nEnc );
        DBG_ERRORFILE( aMsg.GetBuffer() );
#endif
    }
}

//------------------------------------------------------------------------

void SvxTextEncodingBox::RemoveTextEncoding( const rtl_TextEncoding nEnc )
{
    USHORT nAt = EncodingToPos_Impl( nEnc );

    if ( nAt != LISTBOX_ENTRY_NOTFOUND )
        RemoveEntry( nAt );
}

//------------------------------------------------------------------------

rtl_TextEncoding SvxTextEncodingBox::GetSelectTextEncoding() const
{
    USHORT nPos = GetSelectEntryPos();

    if ( nPos != LISTBOX_ENTRY_NOTFOUND )
        return rtl_TextEncoding( (ULONG)GetEntryData(nPos) );
    else
        return RTL_TEXTENCODING_DONTKNOW;
}

//------------------------------------------------------------------------

void SvxTextEncodingBox::SelectTextEncoding( const rtl_TextEncoding nEnc, BOOL bSelect )
{
    USHORT nAt = EncodingToPos_Impl( nEnc );

    if ( nAt != LISTBOX_ENTRY_NOTFOUND )
        SelectEntryPos( nAt, bSelect );
}

//------------------------------------------------------------------------

BOOL SvxTextEncodingBox::IsTextEncodingSelected( const rtl_TextEncoding nEnc ) const
{
    USHORT nAt = EncodingToPos_Impl( nEnc );

    if ( nAt != LISTBOX_ENTRY_NOTFOUND )
        return IsEntryPosSelected( nAt );
    else
        return FALSE;
}

