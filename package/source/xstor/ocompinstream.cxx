/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ocompinstream.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 17:24:11 $
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
#include "precompiled_package.hxx"

#include "ocompinstream.hxx"

#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#include "owriteablestream.hxx"

using namespace ::com::sun::star;

//-----------------------------------------------
OInputCompStream::OInputCompStream()
: m_pImpl( NULL )
, m_rMutexRef( new SotMutexHolder )
, m_pInterfaceContainer( NULL )
, m_bDisposed( sal_False )
{
}

//-----------------------------------------------
OInputCompStream::OInputCompStream( OWriteStream_Impl& aImpl )
: m_pImpl( &aImpl )
, m_rMutexRef( m_pImpl->m_rMutexRef )
, m_pInterfaceContainer( NULL )
, m_bDisposed( sal_False )
{
    OSL_ENSURE( m_pImpl->m_rMutexRef.Is(), "No mutex is provided!\n" );
    if ( !m_pImpl->m_rMutexRef.Is() )
        throw uno::RuntimeException(); // just a disaster
}

//-----------------------------------------------
OInputCompStream::OInputCompStream( OWriteStream_Impl& aImpl,
                                    uno::Reference < io::XInputStream > xStream,
                                    const uno::Sequence< beans::PropertyValue >& aProps )
: m_pImpl( &aImpl )
, m_rMutexRef( m_pImpl->m_rMutexRef )
, m_xStream( xStream )
, m_pInterfaceContainer( NULL )
, m_aProperties( aProps )
, m_bDisposed( sal_False )
{
    OSL_ENSURE( m_pImpl->m_rMutexRef.Is(), "No mutex is provided!\n" );
    if ( !m_pImpl->m_rMutexRef.Is() )
        throw uno::RuntimeException(); // just a disaster

    OSL_ENSURE( xStream.is(), "No stream is provided!\n" );
}

//-----------------------------------------------
OInputCompStream::OInputCompStream( uno::Reference < io::XInputStream > xStream,
                                    const uno::Sequence< beans::PropertyValue >& aProps )
: m_pImpl( NULL )
, m_rMutexRef( new SotMutexHolder )
, m_xStream( xStream )
, m_pInterfaceContainer( NULL )
, m_aProperties( aProps )
, m_bDisposed( sal_False )
{
    OSL_ENSURE( xStream.is(), "No stream is provided!\n" );
}

//-----------------------------------------------
OInputCompStream::~OInputCompStream()
{
    {
        ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );

        if ( !m_bDisposed )
        {
            m_refCount++;
            dispose();
        }

        if ( m_pInterfaceContainer )
            delete m_pInterfaceContainer;
    }
}

//-----------------------------------------------
sal_Int32 SAL_CALL OInputCompStream::readBytes( uno::Sequence< sal_Int8 >& aData, sal_Int32 nBytesToRead )
        throw ( io::NotConnectedException,
                io::BufferSizeExceededException,
                io::IOException,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );
    if ( m_bDisposed )
        throw lang::DisposedException();

    if ( !m_xStream.is() )
        throw uno::RuntimeException();

    return m_xStream->readBytes( aData, nBytesToRead );
}

//-----------------------------------------------
sal_Int32 SAL_CALL OInputCompStream::readSomeBytes( uno::Sequence< sal_Int8 >& aData, sal_Int32 nMaxBytesToRead )
        throw ( io::NotConnectedException,
                io::BufferSizeExceededException,
                io::IOException,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );
    if ( m_bDisposed )
        throw lang::DisposedException();

    if ( !m_xStream.is() )
        throw uno::RuntimeException();

    return m_xStream->readSomeBytes( aData, nMaxBytesToRead );

}

//-----------------------------------------------
void SAL_CALL OInputCompStream::skipBytes( sal_Int32 nBytesToSkip )
        throw ( io::NotConnectedException,
                io::BufferSizeExceededException,
                io::IOException,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );
    if ( m_bDisposed )
        throw lang::DisposedException();

    if ( !m_xStream.is() )
        throw uno::RuntimeException();

    m_xStream->skipBytes( nBytesToSkip );

}

//-----------------------------------------------
sal_Int32 SAL_CALL OInputCompStream::available(  )
        throw ( io::NotConnectedException,
                io::IOException,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );
    if ( m_bDisposed )
        throw lang::DisposedException();

    if ( !m_xStream.is() )
        throw uno::RuntimeException();

    return m_xStream->available();

}

//-----------------------------------------------
void SAL_CALL OInputCompStream::closeInput(  )
        throw ( io::NotConnectedException,
                io::IOException,
                uno::RuntimeException )
{
    dispose();
}

//-----------------------------------------------
uno::Reference< io::XInputStream > SAL_CALL OInputCompStream::getInputStream()
        throw ( uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );
    if ( m_bDisposed )
        throw lang::DisposedException();

    if ( !m_xStream.is() )
        return uno::Reference< io::XInputStream >();

    return uno::Reference< io::XInputStream >( static_cast< io::XInputStream* >( this ), uno::UNO_QUERY );
}

//-----------------------------------------------
uno::Reference< io::XOutputStream > SAL_CALL OInputCompStream::getOutputStream()
        throw ( uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );
    if ( m_bDisposed )
        throw lang::DisposedException();

    return uno::Reference< io::XOutputStream >();
}

//-----------------------------------------------
void OInputCompStream::InternalDispose()
{
    // can be called only by OWriteStream_Impl
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );
    if ( m_bDisposed )
        throw lang::DisposedException();

    if ( m_pInterfaceContainer )
    {
        lang::EventObject aSource( static_cast< ::cppu::OWeakObject*>( this ) );
        m_pInterfaceContainer->disposeAndClear( aSource );
    }

    m_xStream->closeInput();

    m_pImpl = NULL;
    m_bDisposed = sal_True;
}

//-----------------------------------------------
void SAL_CALL OInputCompStream::dispose(  )
        throw ( uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );
    if ( m_bDisposed )
        throw lang::DisposedException();

    if ( m_pInterfaceContainer )
    {
        lang::EventObject aSource( static_cast< ::cppu::OWeakObject*>( this ) );
        m_pInterfaceContainer->disposeAndClear( aSource );
    }

    m_xStream->closeInput();

    if ( m_pImpl )
    {
        m_pImpl->InputStreamDisposed( this );
        m_pImpl = NULL;
    }

    m_bDisposed = sal_True;
}

//-----------------------------------------------
void SAL_CALL OInputCompStream::addEventListener( const uno::Reference< lang::XEventListener >& xListener )
        throw ( uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );
    if ( m_bDisposed )
        throw lang::DisposedException();

    if ( !m_pInterfaceContainer )
        m_pInterfaceContainer = new ::cppu::OInterfaceContainerHelper( m_rMutexRef->GetMutex() );

    m_pInterfaceContainer->addInterface( xListener );
}

//-----------------------------------------------
void SAL_CALL OInputCompStream::removeEventListener( const uno::Reference< lang::XEventListener >& xListener )
        throw ( uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );
    if ( m_bDisposed )
        throw lang::DisposedException();

    if ( m_pInterfaceContainer )
        m_pInterfaceContainer->removeInterface( xListener );
}

//-----------------------------------------------
uno::Reference< beans::XPropertySetInfo > SAL_CALL OInputCompStream::getPropertySetInfo()
        throw ( uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );

    if ( m_bDisposed )
        throw lang::DisposedException();

    //TODO:
    return uno::Reference< beans::XPropertySetInfo >();
}

//-----------------------------------------------
void SAL_CALL OInputCompStream::setPropertyValue( const ::rtl::OUString& /*aPropertyName*/, const uno::Any& /*aValue*/ )
        throw ( beans::UnknownPropertyException,
                beans::PropertyVetoException,
                lang::IllegalArgumentException,
                lang::WrappedTargetException,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );

    if ( m_bDisposed )
        throw lang::DisposedException();

    throw beans::PropertyVetoException(); // TODO
}


//-----------------------------------------------
uno::Any SAL_CALL OInputCompStream::getPropertyValue( const ::rtl::OUString& aProp )
        throw ( beans::UnknownPropertyException,
                lang::WrappedTargetException,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );

    if ( m_bDisposed )
        throw lang::DisposedException();

    ::rtl::OUString aPropertyName;
    if ( aProp.equalsAscii( "IsEncrypted" ) )
        aPropertyName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Encrypted" ) );
    else
        aPropertyName = aProp;

    if ( aPropertyName.equalsAscii( "MediaType" )
      || aPropertyName.equalsAscii( "Size" )
      || aPropertyName.equalsAscii( "Encrypted" )
      || aPropertyName.equalsAscii( "Compressed" )
      || aPropertyName.equalsAscii( "UseCommonStoragePasswordEncryption" ) )
    {
        for ( sal_Int32 aInd = 0; aInd < m_aProperties.getLength(); aInd++ )
        {
            if ( m_aProperties[aInd].Name.equals( aPropertyName ) )
            {
                return m_aProperties[aInd].Value;
            }
        }
    }
    else if ( aPropertyName.equalsAscii( "EncryptionKey" ) )
        throw lang::WrappedTargetException(); // TODO: PropertyVetoException

    throw beans::UnknownPropertyException();
}


//-----------------------------------------------
void SAL_CALL OInputCompStream::addPropertyChangeListener(
    const ::rtl::OUString& /*aPropertyName*/,
    const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/ )
        throw ( beans::UnknownPropertyException,
                lang::WrappedTargetException,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );

    if ( m_bDisposed )
        throw lang::DisposedException();

    //TODO:
}


//-----------------------------------------------
void SAL_CALL OInputCompStream::removePropertyChangeListener(
    const ::rtl::OUString& /*aPropertyName*/,
    const uno::Reference< beans::XPropertyChangeListener >& /*aListener*/ )
        throw ( beans::UnknownPropertyException,
                lang::WrappedTargetException,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );

    if ( m_bDisposed )
        throw lang::DisposedException();

    //TODO:
}


//-----------------------------------------------
void SAL_CALL OInputCompStream::addVetoableChangeListener(
    const ::rtl::OUString& /*PropertyName*/,
    const uno::Reference< beans::XVetoableChangeListener >& /*aListener*/ )
        throw ( beans::UnknownPropertyException,
                lang::WrappedTargetException,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );

    if ( m_bDisposed )
        throw lang::DisposedException();

    //TODO:
}


//-----------------------------------------------
void SAL_CALL OInputCompStream::removeVetoableChangeListener(
    const ::rtl::OUString& /*PropertyName*/,
    const uno::Reference< beans::XVetoableChangeListener >& /*aListener*/ )
        throw ( beans::UnknownPropertyException,
                lang::WrappedTargetException,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_rMutexRef->GetMutex() );

    if ( m_bDisposed )
        throw lang::DisposedException();

    //TODO:
}


