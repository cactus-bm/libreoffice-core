/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ownsubfilterservice.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 23:24:38 $
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
#include "precompiled_sfx2.hxx"

#ifndef _COM_SUN_STAR_FRAME_DOUBLEINITIALIZATIONEXCEPTION_HPP_
#include <com/sun/star/frame/DoubleInitializationException.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif

#include <ownsubfilterservice.hxx>
#include <sfx2/objsh.hxx>

using namespace ::com::sun::star;

namespace sfx2 {

//-------------------------------------------------------------------------
OwnSubFilterService::OwnSubFilterService( const uno::Reference < lang::XMultiServiceFactory >& xFactory )
: m_xFactory( xFactory )
, m_pObjectShell( NULL )
{
}

//-------------------------------------------------------------------------
OwnSubFilterService::~OwnSubFilterService()
{
}

//-------------------------------------------------------------------------
uno::Sequence< ::rtl::OUString > SAL_CALL OwnSubFilterService::impl_getStaticSupportedServiceNames()
{
    uno::Sequence< ::rtl::OUString > aRet(2);
    aRet[0] = ::rtl::OUString::createFromAscii("com.sun.star.document.OwnSubFilter");
    aRet[1] = ::rtl::OUString::createFromAscii("com.sun.star.comp.document.OwnSubFilter");
    return aRet;
}

//-------------------------------------------------------------------------
::rtl::OUString SAL_CALL OwnSubFilterService::impl_getStaticImplementationName()
{
    return ::rtl::OUString::createFromAscii("com.sun.star.comp.document.OwnSubFilter");
}

//-------------------------------------------------------------------------
uno::Reference< uno::XInterface > SAL_CALL OwnSubFilterService::impl_staticCreateSelfInstance(
            const uno::Reference< lang::XMultiServiceFactory >& xServiceManager )
{
    return uno::Reference< uno::XInterface >( *new OwnSubFilterService( xServiceManager ) );
}

//-------------------------------------------------------------------------
uno::Reference< lang::XSingleServiceFactory > SAL_CALL OwnSubFilterService::impl_createFactory(
            const uno::Reference< lang::XMultiServiceFactory >& xServiceManager )
{
    return ::cppu::createSingleFactory( xServiceManager,
                                OwnSubFilterService::impl_getStaticImplementationName(),
                                OwnSubFilterService::impl_staticCreateSelfInstance,
                                OwnSubFilterService::impl_getStaticSupportedServiceNames() );
}


// XFilter

//-------------------------------------------------------------------------
sal_Bool SAL_CALL OwnSubFilterService::filter( const uno::Sequence< beans::PropertyValue >& aDescriptor )
    throw (uno::RuntimeException)
{
    if ( !m_pObjectShell )
        throw uno::RuntimeException();

    return m_pObjectShell->ImportFromGeneratedStream_Impl( m_xStream, aDescriptor );
}

//-------------------------------------------------------------------------
void SAL_CALL OwnSubFilterService::cancel()
    throw (uno::RuntimeException)
{
    // not implemented
}


// XInitialization

//-------------------------------------------------------------------------
void SAL_CALL OwnSubFilterService::initialize( const uno::Sequence< uno::Any >& aArguments )
    throw (uno::Exception, uno::RuntimeException)
{
    if ( !m_xFactory.is() )
        throw uno::RuntimeException();

    if ( aArguments.getLength() != 2 )
        throw lang::IllegalArgumentException();

    if ( m_pObjectShell )
        throw frame::DoubleInitializationException();

    if ( ( aArguments[1] >>= m_xStream ) && m_xStream.is()
      && ( aArguments[0] >>= m_xModel ) && m_xModel.is() )
    {
        ::com::sun::star::uno::Reference < ::com::sun::star::lang::XUnoTunnel > xObj( m_xModel, uno::UNO_QUERY_THROW );
        ::com::sun::star::uno::Sequence < sal_Int8 > aSeq( SvGlobalName( SFX_GLOBAL_CLASSID ).GetByteSequence() );
        sal_Int64 nHandle = xObj->getSomething( aSeq );
        if ( nHandle )
            m_pObjectShell = reinterpret_cast< SfxObjectShell* >( sal::static_int_cast< sal_IntPtr >( nHandle ));
    }

    if ( !m_pObjectShell )
        throw lang::IllegalArgumentException();
}


// XServiceInfo

//-------------------------------------------------------------------------
::rtl::OUString SAL_CALL OwnSubFilterService::getImplementationName()
    throw ( uno::RuntimeException )
{
    return impl_getStaticImplementationName();
}

//-------------------------------------------------------------------------
sal_Bool SAL_CALL OwnSubFilterService::supportsService( const ::rtl::OUString& ServiceName )
    throw ( uno::RuntimeException )
{
    uno::Sequence< ::rtl::OUString > aSeq = impl_getStaticSupportedServiceNames();

    for ( sal_Int32 nInd = 0; nInd < aSeq.getLength(); nInd++ )
        if ( ServiceName.compareTo( aSeq[nInd] ) == 0 )
            return sal_True;

    return sal_False;
}

//-------------------------------------------------------------------------
uno::Sequence< ::rtl::OUString > SAL_CALL OwnSubFilterService::getSupportedServiceNames()
    throw ( uno::RuntimeException )
{
    return impl_getStaticSupportedServiceNames();
}

} // namespace sfx2

