/*************************************************************************
 *
 *  $RCSfile: Deflater.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: vg $ $Date: 2003-12-17 18:02:09 $
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
 *  Contributor(s): Martin Gallwey (gallwey@sun.com)
 *
 *
 ************************************************************************/
#ifndef _DEFLATER_HXX_
#include <Deflater.hxx>
#endif
#ifndef _ZLIB_H
#ifdef SYSTEM_ZLIB
#include <zlib.h>
#else
#include <external/zlib/zlib.h>
#endif
#endif
#ifndef _VOS_DIAGNOSE_H_
#include <vos/diagnose.hxx>
#endif
#ifndef _COM_SUN_STAR_PACKAGES_ZIP_ZIPCONSTANTS_HPP_
#include <com/sun/star/packages/zip/ZipConstants.hpp>
#endif
#include <string.h> // for memset

using namespace com::sun::star::packages::zip::ZipConstants;
using namespace com::sun::star;

/** Provides general purpose compression using the ZLIB compression
 * library.
 */

Deflater::~Deflater(void)
{
    end();
}
void Deflater::init (sal_Int32 nLevel, sal_Int32 nStrategy, sal_Bool bNowrap)
{
    pStream = new z_stream;
    /* Memset it to 0...sets zalloc/zfree/opaque to NULL */
    memset (pStream, 0, sizeof(*pStream));

    switch (deflateInit2(pStream, nLevel, Z_DEFLATED, bNowrap? -MAX_WBITS : MAX_WBITS,
                DEF_MEM_LEVEL, nStrategy))
    {
        case Z_OK:
            break;
        case Z_MEM_ERROR:
            delete pStream;
            break;
        case Z_STREAM_ERROR:
            delete pStream;
            break;
        default:
             break;
    }
}
Deflater::Deflater()
: nLevel(DEFAULT_COMPRESSION)
, nStrategy(DEFAULT_STRATEGY)
, bFinish(sal_False)
, bFinished(sal_False)
, bSetParams(sal_False)
, nOffset(0)
, nLength(0)
{
    init(DEFAULT_COMPRESSION, DEFAULT_STRATEGY, sal_False);
}

Deflater::Deflater(sal_Int32 nSetLevel)
: nLevel(nSetLevel)
, nStrategy(DEFAULT_STRATEGY)
, bFinish(sal_False)
, bFinished(sal_False)
, bSetParams(sal_False)
, nOffset(0)
, nLength(0)
{
    init(nSetLevel, DEFAULT_STRATEGY, sal_False);
}

Deflater::Deflater(sal_Int32 nSetLevel, sal_Bool bNowrap)
: nLevel(nSetLevel)
, nStrategy(DEFAULT_STRATEGY)
, bFinish(sal_False)
, bFinished(sal_False)
, bSetParams(sal_False)
, nOffset(0)
, nLength(0)
{
    init(nSetLevel, DEFAULT_STRATEGY, bNowrap);
}

sal_Int32 Deflater::doDeflateBytes (uno::Sequence < sal_Int8 > &rBuffer, sal_Int32 nNewOffset, sal_Int32 nNewLength)
{
    sal_Int32 nResult;
    if (bSetParams)
    {
        pStream->next_in   = (unsigned char*) sInBuffer.getConstArray();
        pStream->next_out  = (unsigned char*) rBuffer.getArray()+nNewOffset;
        pStream->avail_in  = nLength;
        pStream->avail_out = nNewLength;

#ifdef SYSTEM_ZLIB
        nResult = deflateParams(pStream, nLevel, nStrategy);
#else
        nResult = z_deflateParams(pStream, nLevel, nStrategy);
#endif
        switch (nResult)
        {
            case Z_OK:
                bSetParams = sal_False;
                nOffset += nLength - pStream->avail_in;
                nLength = pStream->avail_in;
                return nNewLength - pStream->avail_out;
            case Z_BUF_ERROR:
                bSetParams = sal_False;
                VOS_DEBUG_ONLY( pStream->msg );
                return 0;
            default:
                VOS_DEBUG_ONLY( pStream->msg );
                return 0;
        }
    }
    else
    {
        pStream->next_in   = (unsigned char*) sInBuffer.getConstArray();
        pStream->next_out  = (unsigned char*) rBuffer.getArray()+nNewOffset;
        pStream->avail_in  = nLength;
        pStream->avail_out = nNewLength;

#ifdef SYSTEM_ZLIB
        nResult = deflate(pStream, bFinish ? Z_FINISH : Z_NO_FLUSH);
#else
        nResult = z_deflate(pStream, bFinish ? Z_FINISH : Z_NO_FLUSH);
#endif
        switch (nResult)
        {
            case Z_STREAM_END:
                bFinished = sal_True;
            case Z_OK:
                nOffset += nLength - pStream->avail_in;
                nLength = pStream->avail_in;
                return nNewLength - pStream->avail_out;
            case Z_BUF_ERROR:
                bSetParams = sal_False;
                VOS_DEBUG_ONLY( pStream->msg );
                return 0;
            default:
                VOS_DEBUG_ONLY( pStream->msg );
                return 0;
        }
    }
}

void SAL_CALL Deflater::setInputSegment( const uno::Sequence< sal_Int8 >& rBuffer, sal_Int32 nNewOffset, sal_Int32 nNewLength )
{
    if (nNewOffset < 0 || nNewLength < 0 || nNewOffset + nNewLength > rBuffer.getLength())
        VOS_DEBUG_ONLY("Invalid parameters to Deflater::setInputSegment!");

    sInBuffer = rBuffer;
    nOffset = nNewOffset;
    nLength = nNewLength;
}
void SAL_CALL Deflater::setInput( const uno::Sequence< sal_Int8 >& rBuffer )
{
    sInBuffer = rBuffer;
    nOffset = 0;
    nLength = rBuffer.getLength();
}
void SAL_CALL Deflater::setDictionarySegment( const uno::Sequence< sal_Int8 >& rBuffer, sal_Int32 nNewOffset, sal_Int32 nNewLength )
{
    if (pStream == NULL)
    {
        // do error handling
        VOS_DEBUG_ONLY("No stream!");
    }
    if (nNewOffset < 0 || nNewLength < 0 || nNewOffset + nNewLength > rBuffer.getLength())
    {
        // do error handling
    }
#ifdef SYSTEM_ZLIB
    sal_Int32 nResult = deflateSetDictionary(pStream, (const unsigned char*)rBuffer.getConstArray()+nOffset, nLength);
#else
    sal_Int32 nResult = z_deflateSetDictionary(pStream, (const unsigned char*)rBuffer.getConstArray()+nOffset, nLength);
#endif
}
void SAL_CALL Deflater::setDictionary( const uno::Sequence< sal_Int8 >& rBuffer )
{
    if (pStream == NULL)
    {
        // do error handling
        VOS_DEBUG_ONLY("No stream!");

    }
#ifdef SYSTEM_ZLIB
    sal_Int32 nResult = deflateSetDictionary(pStream, (const unsigned char*)rBuffer.getConstArray(), rBuffer.getLength());
#else
    sal_Int32 nResult = z_deflateSetDictionary(pStream, (const unsigned char*)rBuffer.getConstArray(), rBuffer.getLength());
#endif
}
void SAL_CALL Deflater::setStrategy( sal_Int32 nNewStrategy )
{
    if (nNewStrategy != DEFAULT_STRATEGY &&
        nNewStrategy != FILTERED &&
        nNewStrategy != HUFFMAN_ONLY)
    {
        // do error handling
    }
    if (nStrategy != nNewStrategy)
    {
        nStrategy = nNewStrategy;
        bSetParams = sal_True;
    }
}
void SAL_CALL Deflater::setLevel( sal_Int32 nNewLevel )
{
    if ((nNewLevel < 0 || nNewLevel > 9) && nNewLevel != DEFAULT_COMPRESSION)
    {
        // do error handling
    }
    if (nNewLevel != nLevel)
    {
        nLevel = nNewLevel;
        bSetParams = sal_True;
    }
}
sal_Bool SAL_CALL Deflater::needsInput(  )
{
    return nLength <=0;
}
void SAL_CALL Deflater::finish(  )
{
    bFinish = sal_True;
}
sal_Bool SAL_CALL Deflater::finished(  )
{
    return bFinished;
}
sal_Int32 SAL_CALL Deflater::doDeflateSegment( uno::Sequence< sal_Int8 >& rBuffer, sal_Int32 nNewOffset, sal_Int32 nNewLength )
{
    if (nNewOffset < 0 || nNewLength < 0 || nNewOffset + nNewLength > rBuffer.getLength())
        VOS_DEBUG_ONLY("Invalid Offset or Length passed to doDeflateSegment");
    return doDeflateBytes(rBuffer, nNewOffset, nNewLength);
}
sal_Int32 SAL_CALL Deflater::doDeflate( uno::Sequence< sal_Int8 >& rBuffer )
{
    return doDeflateBytes(rBuffer, 0, rBuffer.getLength());
}

sal_Int32 SAL_CALL Deflater::getAdler(  )
{
    return pStream->adler;
}
sal_Int32 SAL_CALL Deflater::getTotalIn(  )
{
    return pStream->total_in;
}
sal_Int32 SAL_CALL Deflater::getTotalOut(  )
{
    return pStream->total_out;
}
void SAL_CALL Deflater::reset(  )
{
#ifdef SYSTEM_ZLIB
    deflateReset(pStream);
#else
    z_deflateReset(pStream);
#endif
    bFinish = sal_False;
    bFinished = sal_False;
    nOffset = nLength = 0;
}
void SAL_CALL Deflater::end(  )
{
    if (pStream != NULL)
    {
#ifdef SYSTEM_ZLIB
        deflateEnd(pStream);
#else
        z_deflateEnd(pStream);
#endif
        delete pStream;
    }
    pStream = NULL;
}
