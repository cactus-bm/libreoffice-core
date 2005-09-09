/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: utf8conv.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 15:03:24 $
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
#include "utf8conv.hxx"

//
// class UTF8Converter
//

#define MAX_CONV_BUFFER_SIZE    0xFF00

#define TO_CVTFLAGS     (RTL_TEXTTOUNICODE_FLAGS_UNDEFINED_MAPTOPRIVATE |\
                          RTL_TEXTTOUNICODE_FLAGS_MBUNDEFINED_DEFAULT |\
                          RTL_TEXTTOUNICODE_FLAGS_INVALID_DEFAULT)

#define FROM_CVTFLAGS   (RTL_UNICODETOTEXT_FLAGS_UNDEFINED_DEFAULT |\
                             RTL_UNICODETOTEXT_FLAGS_INVALID_DEFAULT |\
                            RTL_UNICODETOTEXT_FLAGS_UNDEFINED_REPLACE |\
                            RTL_UNICODETOTEXT_FLAGS_PRIVATE_MAPTO0)

/*****************************************************************************/
void UTF8Converter::Convert( ByteString &rBuffer,
    rtl_TextEncoding nSourceENC, rtl_TextEncoding nDestENC )
/*****************************************************************************/
{
    String sTemp( rBuffer, nSourceENC );
    rBuffer = ByteString( sTemp, nDestENC );
}

/*****************************************************************************/
ByteString UTF8Converter::ConvertToUTF8(
    const ByteString &rASCII, rtl_TextEncoding nEncoding )
/*****************************************************************************/
{
    ByteString sReturn( rASCII );
    Convert( sReturn, nEncoding, RTL_TEXTENCODING_UTF8 );
    return sReturn;
}

/*****************************************************************************/
ByteString UTF8Converter::ConvertFromUTF8(
    const ByteString &rUTF8, rtl_TextEncoding nEncoding )
/*****************************************************************************/
{
    ByteString sReturn( rUTF8 );
    Convert( sReturn, RTL_TEXTENCODING_UTF8, nEncoding );
    return sReturn;
}
