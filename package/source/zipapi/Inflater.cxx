/*************************************************************************
 *
 *  $RCSfile: Inflater.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: mtg $ $Date: 2001-11-15 20:17:02 $
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
#ifndef _INFLATER_HXX_
#include <Inflater.hxx>
#endif
#ifndef _ZLIB_H
#include <external/zlib/zlib.h>
#endif
#ifndef _VOS_DIAGNOSE_H_
#include <vos/diagnose.hxx>
#endif
#include <string.h> // for memset

using namespace com::sun::star::uno;

/** Provides general purpose decompression using the ZLIB library */

Inflater::Inflater(sal_Bool bNoWrap)
: bFinish(sal_False),
  bFinished(sal_False),
  bSetParams(sal_False),
  bNeedDict(sal_False),
  nOffset(0),
  nLength(0),
  pStream(NULL)
{
    pStream = new z_stream;
    /* memset to 0 to set zalloc/opaque etc */
    memset (pStream, 0, sizeof(*pStream));
    sal_Int32 nRes;
    nRes = inflateInit2(pStream, bNoWrap ? -MAX_WBITS : MAX_WBITS);
    switch (nRes)
    {
        case Z_OK:
            break;
        case Z_MEM_ERROR:
            VOS_DEBUG_ONLY ( pStream->msg);
            delete pStream;
            break;
        case Z_STREAM_ERROR:
            VOS_DEBUG_ONLY ( pStream->msg);
            delete pStream;
            break;
        default:
            VOS_DEBUG_ONLY ( pStream->msg);
            break;
    }
}

Inflater::~Inflater()
{
    end();
}
void SAL_CALL Inflater::setInputSegment( const Sequence< sal_Int8 >& rBuffer, sal_Int32 nNewOffset, sal_Int32 nNewLength )
{
    sInBuffer = rBuffer;
    nOffset = nNewOffset;
    nLength = nNewLength;
}

void SAL_CALL Inflater::setInput( const Sequence< sal_Int8 >& rBuffer )
{
    sInBuffer = rBuffer;
    nOffset = 0;
    nLength = rBuffer.getLength();
}

void SAL_CALL Inflater::setDictionarySegment( const Sequence< sal_Int8 >& rBuffer, sal_Int32 nNewOffset, sal_Int32 nNewLength )
{
    if (pStream == NULL)
    {
        // do error handling
    }
    if (nNewOffset < 0 || nNewLength < 0 || nNewOffset + nNewLength > rBuffer.getLength())
    {
        // do error handling
    }
    z_inflateSetDictionary(pStream, (const unsigned char*)rBuffer.getConstArray() + nNewOffset,
                  nNewLength);
}

void SAL_CALL Inflater::setDictionary( const Sequence< sal_Int8 >& rBuffer )
{
    if (pStream == NULL)
    {
        // do error handling
    }
    z_inflateSetDictionary(pStream, (const unsigned char*)rBuffer.getConstArray(),
                   rBuffer.getLength());
}

sal_Int32 SAL_CALL Inflater::getRemaining(  )
{
    return nLength;
}

sal_Bool SAL_CALL Inflater::needsInput(  )
{
    return nLength <=0;
}

sal_Bool SAL_CALL Inflater::needsDictionary(  )
{
    return bNeedDict;
}

void SAL_CALL Inflater::finish(  )
{
    bFinish = sal_True;
}
sal_Bool SAL_CALL Inflater::finished(  )
{
    return bFinished;
}

sal_Int32 SAL_CALL Inflater::doInflateSegment( Sequence< sal_Int8 >& rBuffer, sal_Int32 nNewOffset, sal_Int32 nNewLength )
{
    if (nNewOffset < 0 || nNewLength < 0 || nNewOffset + nNewLength > rBuffer.getLength())
    {
        // do error handling
    }
    return doInflateBytes(rBuffer, nNewOffset, nNewLength);
}

sal_Int32 SAL_CALL Inflater::doInflate( Sequence< sal_Int8 >& rBuffer )
{
    return doInflateBytes(rBuffer, 0, rBuffer.getLength());
}

sal_Int32 SAL_CALL Inflater::getAdler(  )
{
    return pStream->adler;
}

sal_Int32 SAL_CALL Inflater::getTotalIn(  )
{
    return pStream->total_in;
}

sal_Int32 SAL_CALL Inflater::getTotalOut(  )
{
    return pStream->total_out;
}

void SAL_CALL Inflater::reset(  )
{
    z_inflateReset(pStream);
    bFinish = bNeedDict = bFinished = sal_False;
    nOffset = nLength = 0;
}

void SAL_CALL Inflater::end(  )
{
    if (pStream != NULL)
    {
        z_inflateEnd(pStream);
        delete pStream;
    }
    pStream = NULL;
}

sal_Int32 Inflater::doInflateBytes (Sequence < sal_Int8 >  &rBuffer, sal_Int32 nNewOffset, sal_Int32 nNewLength)
{
    sal_Int32 nResult;
    pStream->next_in   = ( unsigned char* ) ( sInBuffer.getConstArray() + nOffset );
    pStream->avail_in  = nLength;
    pStream->next_out  = reinterpret_cast < unsigned char* > ( rBuffer.getArray() + nNewOffset );
    pStream->avail_out = nNewLength;

    nResult = ::z_inflate(pStream, bFinish ? Z_SYNC_FLUSH : Z_PARTIAL_FLUSH);

    switch (nResult)
    {
        case Z_STREAM_END:
            bFinished = sal_True;
        case Z_OK:
            nOffset += nLength - pStream->avail_in;
            nLength = pStream->avail_in;
            return nNewLength - pStream->avail_out;
        case Z_NEED_DICT:
            bNeedDict = sal_True;
            nOffset += nLength - pStream->avail_in;
            nLength = pStream->avail_in;
        case Z_BUF_ERROR:
            return 0;
        case Z_DATA_ERROR:
            VOS_DEBUG_ONLY(pStream->msg);
            return 0;
    }
    return 0;
}

