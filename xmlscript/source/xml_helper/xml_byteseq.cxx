/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xml_byteseq.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-20 05:11:06 $
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
#include <rtl/memory.h>

#include <cppuhelper/implbase1.hxx>
#include <xmlscript/xml_helper.hxx>


using namespace rtl;
using namespace osl;
using namespace com::sun::star;
using namespace com::sun::star::uno;


namespace xmlscript
{

//==================================================================================================
class BSeqInputStream
    : public ::cppu::WeakImplHelper1< io::XInputStream >
{
    ByteSequence _seq;
    sal_Int32 _nPos;

public:
    inline BSeqInputStream( ByteSequence const & rSeq )
        SAL_THROW( () )
        : _seq( rSeq )
        , _nPos( 0 )
        {}

    // XInputStream
    virtual sal_Int32 SAL_CALL readBytes(
        Sequence< sal_Int8 > & rData, sal_Int32 nBytesToRead )
        throw (io::NotConnectedException, io::BufferSizeExceededException, io::IOException, RuntimeException);
    virtual sal_Int32 SAL_CALL readSomeBytes(
        Sequence< sal_Int8 > & rData, sal_Int32 nMaxBytesToRead )
        throw (io::NotConnectedException, io::BufferSizeExceededException, io::IOException, RuntimeException);
    virtual void SAL_CALL skipBytes(
        sal_Int32 nBytesToSkip )
        throw (io::NotConnectedException, io::BufferSizeExceededException, io::IOException, RuntimeException);
    virtual sal_Int32 SAL_CALL available()
        throw (io::NotConnectedException, io::IOException, RuntimeException);
    virtual void SAL_CALL closeInput()
        throw (io::NotConnectedException, io::IOException, RuntimeException);
};
//__________________________________________________________________________________________________
sal_Int32 BSeqInputStream::readBytes(
    Sequence< sal_Int8 > & rData, sal_Int32 nBytesToRead )
    throw (io::NotConnectedException, io::BufferSizeExceededException, io::IOException, RuntimeException)
{
    nBytesToRead = ((nBytesToRead > _seq.getLength() - _nPos)
                    ? _seq.getLength() - _nPos
                    : nBytesToRead);

    ByteSequence aBytes( _seq.getConstArray() + _nPos, nBytesToRead );
    rData = toUnoSequence( aBytes );
    _nPos += nBytesToRead;
    return nBytesToRead;
}
//__________________________________________________________________________________________________
sal_Int32 BSeqInputStream::readSomeBytes(
    Sequence< sal_Int8 > & rData, sal_Int32 nMaxBytesToRead )
    throw (io::NotConnectedException, io::BufferSizeExceededException, io::IOException, RuntimeException)
{
    return readBytes( rData, nMaxBytesToRead );
}
//__________________________________________________________________________________________________
void BSeqInputStream::skipBytes(
    sal_Int32 /*nBytesToSkip*/ )
    throw (io::NotConnectedException, io::BufferSizeExceededException, io::IOException, RuntimeException)
{
}
//__________________________________________________________________________________________________
sal_Int32 BSeqInputStream::available()
    throw (io::NotConnectedException, io::IOException, RuntimeException)
{
    return (_seq.getLength() - _nPos);
}
//__________________________________________________________________________________________________
void BSeqInputStream::closeInput()
    throw (io::NotConnectedException, io::IOException, RuntimeException)
{
}

//##################################################################################################

//==================================================================================================
class BSeqOutputStream
    : public ::cppu::WeakImplHelper1< io::XOutputStream >
{
    ByteSequence * _seq;

public:
    inline BSeqOutputStream( ByteSequence * seq )
        SAL_THROW( () )
        : _seq( seq )
        {}

    // XOutputStream
    virtual void SAL_CALL writeBytes(
        Sequence< sal_Int8 > const & rData )
        throw (io::NotConnectedException, io::BufferSizeExceededException, RuntimeException);
    virtual void SAL_CALL flush()
        throw (io::NotConnectedException, io::BufferSizeExceededException, RuntimeException);
    virtual void SAL_CALL closeOutput()
        throw (io::NotConnectedException, io::BufferSizeExceededException, RuntimeException);
};
//__________________________________________________________________________________________________
void BSeqOutputStream::writeBytes( Sequence< sal_Int8 > const & rData )
    throw (io::NotConnectedException, io::BufferSizeExceededException, RuntimeException)
{
    sal_Int32 nPos = _seq->getLength();
    _seq->realloc( nPos + rData.getLength() );
    ::rtl_copyMemory( (char *)_seq->getArray() + nPos,
                      (char const *)rData.getConstArray(),
                      rData.getLength() );
}
//__________________________________________________________________________________________________
void BSeqOutputStream::flush()
    throw (io::NotConnectedException, io::BufferSizeExceededException, RuntimeException)
{
}
//__________________________________________________________________________________________________
void BSeqOutputStream::closeOutput()
    throw (io::NotConnectedException, io::BufferSizeExceededException, RuntimeException)
{
}

//##################################################################################################

//==================================================================================================
Reference< io::XInputStream > SAL_CALL createInputStream( ByteSequence const & rInData )
    SAL_THROW( () )
{
    return new BSeqInputStream( rInData );
}

//==================================================================================================
Reference< io::XOutputStream > SAL_CALL createOutputStream( ByteSequence * pOutData )
    SAL_THROW( () )
{
    return new BSeqOutputStream( pOutData );
}

}
