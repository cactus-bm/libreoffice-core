/*************************************************************************
 *
 *  $RCSfile: EntryInputStream.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: mtg $ $Date: 2000-11-21 17:57:07 $
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
#ifndef _ENTRY_INPUT_STREAM_HXX
#include "EntryInputStream.hxx"
#endif

#ifndef _COM_SUN_STAR_PACKAGE_ZIPCONSTANTS_HPP_
#include <com/sun/star/package/ZipConstants.hpp>
#endif

using namespace rtl;
using namespace com::sun::star;

/** Provides access to the compressed data in a zipfile. Decompresses on the fly, but
 * does not currently support XSeekable into the compressed data stream.
 * Acts on the same underlying XInputStream as both the full Zip File and other
 * EntryInputStreams, and thus must maintain its current position in the stream and
 * seek to it before performing any reads.
 */

EntryInputStream::EntryInputStream( uno::Reference < io::XInputStream > xNewInput, sal_Int64 nNewBegin, sal_Int64 nNewEnd, sal_Int32 nNewBufferSize, sal_Int64 nUncompressedSize)
: xStream( xNewInput )
, xSeek( xNewInput, uno::UNO_QUERY )
, nCompBegin( nNewBegin )
, nCompCurrent( nNewBegin )
, nCompEnd( nNewEnd )
, nBegin( 0 )
, nCurrent( 0 )
, nEnd(nUncompressedSize)
, aSequence ( nNewEnd - nNewBegin )
, bReachEOF ( sal_False )
, aInflater( sal_True )
, aBuffer( nUncompressedSize )
{
    if (nCompEnd - nCompBegin < nUncompressedSize)
        bDeflated = sal_True;
    else
        bDeflated = sal_False;
    if (bDeflated)
    {
        xSeek->seek(nNewBegin);
        xStream->readBytes(aSequence, nNewEnd - nNewBegin);
        aInflater.setInputSegment(aSequence, 0, nNewEnd - nNewBegin );
        aInflater.doInflate(aBuffer);
    }
    else
    {
        xSeek->seek(nNewBegin);
        xStream->readBytes(aBuffer, nUncompressedSize);
    }
}

EntryInputStream::~EntryInputStream( void )
{
}
void EntryInputStream::fill(void)
{
    sal_Int32 nLength, nBytesToRead = aSequence.getLength();
    if (nBytesToRead + nCompCurrent> nCompEnd)
        nBytesToRead = nCompEnd - nCompCurrent;
    if (xSeek.is())
        xSeek->seek( nCompCurrent );
    else
        throw (io::IOException());
    nLength = xStream->readBytes(aSequence, nBytesToRead);
    aInflater.setInputSegment(aSequence, 0, nLength);
}
sal_Int32 SAL_CALL EntryInputStream::readBytes( uno::Sequence< sal_Int8 >& aData,
                                        sal_Int32 nBytesToRead )
    throw(io::NotConnectedException, io::BufferSizeExceededException, io::IOException, uno::RuntimeException)
{
    sal_Int64 nDataLen = aData.getLength();

    if (nBytesToRead <=0)
        return 0;

    if (nBytesToRead + nCurrent > nEnd)
    {
        if (nCurrent > nEnd)
            return 0;
        nBytesToRead = nEnd - nCurrent;
    }
    if (nBytesToRead > nDataLen)
        nBytesToRead = nDataLen;

    for ( sal_Int64 i = 0; i< nBytesToRead;i++,nCurrent++)
        aData[i] = aBuffer[nCurrent];

    return nBytesToRead;
    /*
    if (!bDeflated)
    {
        if (xSeek.is())
            xSeek->seek( nCompCurrent );
        sal_Int32 nRead = xStream->readBytes(aData, nBytesToRead );
        nCompCurrent+=nRead;
        return nRead;
    }

    while ( (n = aInflater.doInflate(aData)) == 0)
    {
        if (aInflater.finished() || aInflater.needsDictionary())
        {
            bReachEOF = sal_True;
            return -1;
        }
        if (aInflater.needsInput())
            fill();
    }
    nCurrent+=n;
    return n;
    //return xStream->readBytes(aData, nBytesToRead );
    */
}
sal_Int32 SAL_CALL EntryInputStream::readSomeBytes( uno::Sequence< sal_Int8 >& aData,
                                                sal_Int32 nMaxBytesToRead )
    throw(io::NotConnectedException, io::BufferSizeExceededException, io::IOException, uno::RuntimeException)
{
    /*
    if (nMaxBytesToRead + nCompCurrent > nEnd)
    {
        if (nCurrent > nEnd)
            return 0;
        nMaxBytesToRead = nEnd - nCurrent;
    }
    if (xSeek.is())
        xSeek->seek( nCurrent );
    else
        throw (io::IOException());
    */
    return readBytes( aData, nMaxBytesToRead );
}
void SAL_CALL EntryInputStream::skipBytes( sal_Int32 nBytesToSkip )
    throw(io::NotConnectedException, io::BufferSizeExceededException, io::IOException, uno::RuntimeException)
{
    if (nBytesToSkip == 0)
        return;
    if (nBytesToSkip + nCurrent > nEnd )
        nBytesToSkip = nEnd - nCurrent;

    nCurrent+=nBytesToSkip;
/*
    if (!bDeflated)
    {
        if (nBytesToSkip + nCompCurrent> nCompEnd)
        {
            if (nCurrent> nCompEnd)
                return;
            nBytesToSkip = nCompEnd - nCompCurrent;
        }
        nCompCurrent+=nBytesToSkip;
    }
    else
    {
        sal_Int32 nTotal = 0;
        uno::Sequence < sal_Int8 > aTmpSequence ( 1024 );
        while ( nTotal < nBytesToSkip )
        {
            sal_Int32 nNewLength, nLength = nBytesToSkip - nTotal;
            if ( nLength > aTmpSequence.getLength())
                nLength = aTmpSequence.getLength();
            nNewLength = readSomeBytes(aTmpSequence, nLength);
            if (nNewLength < nLength)
            {
                bReachEOF = sal_True;
                break;
            }
            nTotal +=nNewLength;
        }
        nCurrent+=nBytesToSkip;
    }
*/
}
sal_Int32 SAL_CALL EntryInputStream::available(  )
    throw(io::NotConnectedException, io::IOException, uno::RuntimeException)
{
    return aBuffer.getLength() - nCurrent;
    /*
    if (!bDeflated)
        return nCompEnd - nCompCurrent;
    else
        return nEnd - nCurrent;
    */
}
void SAL_CALL EntryInputStream::closeInput(  )
    throw(io::NotConnectedException, io::IOException, uno::RuntimeException)
{
}

void SAL_CALL EntryInputStream::seek( sal_Int64 location )
    throw(lang::IllegalArgumentException, io::IOException, uno::RuntimeException)
{
    if (location > aBuffer.getLength())
        location = aBuffer.getLength();
    if (location <0)
        location = 0;
    nCurrent = location;
    /*
    if (!bDeflated)
    {
        if (location < nCompBegin)
            location = nCompBegin;
        if (location > nCompEnd)
            location = nCompEnd;
        nCompCurrent = location;
        return;
    }
    else
    {
        if (location == 0)
            nCurrent = nCompCurrent = 0;
        else if (location > nCurrent)
            skipBytes(location - nCurrent);
        else
        {
            nCurrent = 0;
            skipBytes(location);
        }
    }
    */
}
sal_Int64 SAL_CALL EntryInputStream::getPosition(  )
        throw(io::IOException, uno::RuntimeException)
{
    return nCurrent;
    /*
    if (!bDeflated)
        return nCompCurrent;
    else
        return nCurrent;
    */
}
sal_Int64 SAL_CALL EntryInputStream::getLength(  )
        throw(io::IOException, uno::RuntimeException)
{
    return aBuffer.getLength();
    /*
    if (!bDeflated)
        return nCompEnd - nCompBegin;
    else
        return nEnd - nBegin;
    */
}
