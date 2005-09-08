/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: seekableinput.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 03:00:50 $
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

#ifndef _COM_SUN_STAR_IO_XOUTPUTSTREAM_HPP_
#include <com/sun/star/io/XOutputStream.hpp>
#endif


#include <comphelper/seekableinput.hxx>

using namespace ::com::sun::star;

namespace comphelper
{

const sal_Int32 nConstBufferSize = 32000;

//---------------------------------------------------------------------------
void copyInputToOutput_Impl( const uno::Reference< io::XInputStream >& xIn,
                            const uno::Reference< io::XOutputStream >& xOut )
{
    sal_Int32 nRead;
    uno::Sequence< sal_Int8 > aSequence( nConstBufferSize );

    do
    {
        nRead = xIn->readBytes( aSequence, nConstBufferSize );
        if ( nRead < nConstBufferSize )
        {
            uno::Sequence< sal_Int8 > aTempBuf( aSequence.getConstArray(), nRead );
            xOut->writeBytes( aTempBuf );
        }
        else
            xOut->writeBytes( aSequence );
    }
    while ( nRead == nConstBufferSize );
}

//---------------------------------------------------------------------------
OSeekableInputWrapper::OSeekableInputWrapper(
            const uno::Reference< io::XInputStream >& xInStream,
            const uno::Reference< lang::XMultiServiceFactory >& xFactory )
: m_xFactory( xFactory )
, m_xOriginalStream( xInStream )
{
    if ( !m_xFactory.is() )
        throw uno::RuntimeException();
}

//---------------------------------------------------------------------------
OSeekableInputWrapper::~OSeekableInputWrapper()
{
}

//---------------------------------------------------------------------------
uno::Reference< io::XInputStream > OSeekableInputWrapper::CheckSeekableCanWrap(
                            const uno::Reference< io::XInputStream >& xInStream,
                            const uno::Reference< lang::XMultiServiceFactory >& xFactory )
{
    // check that the stream is seekable and just wrap it if it is not
    uno::Reference< io::XSeekable > xSeek( xInStream, uno::UNO_QUERY );
    if ( xSeek.is() )
        return xInStream;

    uno::Reference< io::XInputStream > xNewStream(
            static_cast< io::XInputStream* >(
                new OSeekableInputWrapper( xInStream, xFactory ) ) );
    return xNewStream;
}

//---------------------------------------------------------------------------
void OSeekableInputWrapper::PrepareCopy_Impl()
{
    if ( !m_xCopyInput.is() )
    {
        if ( !m_xFactory.is() )
            throw uno::RuntimeException();

        uno::Reference< io::XOutputStream > xTempOut(
                m_xFactory->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.io.TempFile" ) ),
                uno::UNO_QUERY );

        if ( xTempOut.is() )
        {
            copyInputToOutput_Impl( m_xOriginalStream, xTempOut );
            xTempOut->closeOutput();

            uno::Reference< io::XSeekable > xTempSeek( xTempOut, uno::UNO_QUERY );
            if ( xTempSeek.is() )
            {
                xTempSeek->seek( 0 );
                m_xCopyInput = uno::Reference< io::XInputStream >( xTempOut, uno::UNO_QUERY );
                if ( m_xCopyInput.is() )
                    m_xCopySeek = xTempSeek;
            }
        }
    }

    if ( !m_xCopyInput.is() )
        throw io::IOException();
}

// XInputStream
//---------------------------------------------------------------------------
sal_Int32 SAL_CALL OSeekableInputWrapper::readBytes( uno::Sequence< sal_Int8 >& aData, sal_Int32 nBytesToRead )
    throw ( io::NotConnectedException,
            io::BufferSizeExceededException,
            io::IOException,
            uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );

    if ( !m_xOriginalStream.is() )
        throw io::NotConnectedException();

    PrepareCopy_Impl();

    return m_xCopyInput->readBytes( aData, nBytesToRead );
}

//---------------------------------------------------------------------------
sal_Int32 SAL_CALL OSeekableInputWrapper::readSomeBytes( uno::Sequence< sal_Int8 >& aData, sal_Int32 nMaxBytesToRead )
    throw ( io::NotConnectedException,
            io::BufferSizeExceededException,
            io::IOException,
            uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );

    if ( !m_xOriginalStream.is() )
        throw io::NotConnectedException();

    PrepareCopy_Impl();

    return m_xCopyInput->readSomeBytes( aData, nMaxBytesToRead );
}

//---------------------------------------------------------------------------
void SAL_CALL OSeekableInputWrapper::skipBytes( sal_Int32 nBytesToSkip )
    throw ( io::NotConnectedException,
            io::BufferSizeExceededException,
            io::IOException,
            uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );

    if ( !m_xOriginalStream.is() )
        throw io::NotConnectedException();

    PrepareCopy_Impl();

    m_xCopyInput->skipBytes( nBytesToSkip );
}

//---------------------------------------------------------------------------
sal_Int32 SAL_CALL OSeekableInputWrapper::available()
    throw ( io::NotConnectedException,
            io::IOException,
            uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );

    if ( !m_xOriginalStream.is() )
        throw io::NotConnectedException();

    PrepareCopy_Impl();

    return m_xCopyInput->available();
}

//---------------------------------------------------------------------------
void SAL_CALL OSeekableInputWrapper::closeInput()
    throw ( io::NotConnectedException,
            io::IOException,
            uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );

    if ( !m_xOriginalStream.is() )
        throw io::NotConnectedException();

    m_xOriginalStream->closeInput();
    m_xOriginalStream = uno::Reference< io::XInputStream >();

    if ( m_xCopyInput.is() )
    {
        m_xCopyInput->closeInput();
        m_xCopyInput = uno::Reference< io::XInputStream >();
    }

    m_xCopySeek = uno::Reference< io::XSeekable >();
}


// XSeekable
//---------------------------------------------------------------------------
void SAL_CALL OSeekableInputWrapper::seek( sal_Int64 location )
    throw ( lang::IllegalArgumentException,
            io::IOException,
            uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );

    if ( !m_xOriginalStream.is() )
        throw io::NotConnectedException();

    PrepareCopy_Impl();

    m_xCopySeek->seek( location );
}

//---------------------------------------------------------------------------
sal_Int64 SAL_CALL OSeekableInputWrapper::getPosition()
    throw ( io::IOException,
            uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );

    if ( !m_xOriginalStream.is() )
        throw io::NotConnectedException();

    PrepareCopy_Impl();

    return m_xCopySeek->getPosition();
}

//---------------------------------------------------------------------------
sal_Int64 SAL_CALL OSeekableInputWrapper::getLength()
    throw ( io::IOException,
            uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );

    if ( !m_xOriginalStream.is() )
        throw io::NotConnectedException();

    PrepareCopy_Impl();

    return m_xCopySeek->getLength();
}

}   // namespace comphelper

