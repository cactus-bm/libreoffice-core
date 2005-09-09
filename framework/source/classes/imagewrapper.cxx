/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: imagewrapper.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:11:54 $
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

#include <classes/imagewrapper.hxx>

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

#ifndef _SV_BITMAP_HXX
#include <vcl/bitmap.hxx>
#endif

#ifndef _SV_BITMAPEX_HXX
#include <vcl/bitmapex.hxx>
#endif

#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif

#ifndef _CPPUHELPER_TYPEPROVIDER_HXX_
#include <cppuhelper/typeprovider.hxx>
#endif

using namespace rtl;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;

namespace framework
{

static Sequence< sal_Int8 > impl_getStaticIdentifier()
{
    static sal_uInt8 pGUID[16] = { 0x46, 0xAD, 0x69, 0xFB, 0xA7, 0xBE, 0x44, 0x83, 0xB2, 0xA7, 0xB3, 0xEC, 0x59, 0x4A, 0xB7, 0x00 };
    static ::com::sun::star::uno::Sequence< sal_Int8 > seqID((sal_Int8*)pGUID,16) ;
    return seqID ;
}


ImageWrapper::ImageWrapper( const Image& aImage ) : ThreadHelpBase( &Application::GetSolarMutex() )
                                                    ,   cppu::OWeakObject()
                                                    ,   m_aImage( aImage )
{
}


ImageWrapper::~ImageWrapper()
{
}


Sequence< sal_Int8 > ImageWrapper::GetUnoTunnelId()
{
    return impl_getStaticIdentifier();
}


// XInterface
void SAL_CALL ImageWrapper::acquire() throw ()
{
    OWeakObject::acquire();
}

void SAL_CALL ImageWrapper::release() throw ()
{
    OWeakObject::release();
}

Any SAL_CALL ImageWrapper::queryInterface( const Type& aType )
throw ( RuntimeException )
{
    Any a = ::cppu::queryInterface(
                aType ,
                SAL_STATIC_CAST( com::sun::star::awt::XBitmap*, this ),
                SAL_STATIC_CAST( XUnoTunnel*, this ),
                SAL_STATIC_CAST( XTypeProvider*, this ));

    if( a.hasValue() )
        return a;

    return OWeakObject::queryInterface( aType );
}

// XBitmap
com::sun::star::awt::Size SAL_CALL ImageWrapper::getSize() throw ( RuntimeException )
{
    vos::OGuard aGuard( Application::GetSolarMutex() );

    BitmapEx    aBitmapEx( m_aImage.GetBitmapEx() );
    Size        aBitmapSize( aBitmapEx.GetSizePixel() );

    return com::sun::star::awt::Size( aBitmapSize.Width(), aBitmapSize.Height() );
}

Sequence< sal_Int8 > SAL_CALL ImageWrapper::getDIB() throw ( RuntimeException )
{
    vos::OGuard aGuard( Application::GetSolarMutex() );

    SvMemoryStream aMem;
    aMem << m_aImage.GetBitmapEx().GetBitmap();
    return Sequence< sal_Int8 >( (sal_Int8*) aMem.GetData(), aMem.Tell() );
}

Sequence< sal_Int8 > SAL_CALL ImageWrapper::getMaskDIB() throw ( RuntimeException )
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    BitmapEx    aBmpEx( m_aImage.GetBitmapEx() );

    if ( aBmpEx.IsAlpha() )
    {
        SvMemoryStream aMem;
        aMem << aBmpEx.GetAlpha().GetBitmap();
        return Sequence< sal_Int8 >( (sal_Int8*) aMem.GetData(), aMem.Tell() );
    }
    else if ( aBmpEx.IsTransparent() )
    {
        SvMemoryStream aMem;
        aMem << aBmpEx.GetMask();
        return Sequence< sal_Int8 >( (sal_Int8*) aMem.GetData(), aMem.Tell() );
    }

    return Sequence< sal_Int8 >();
}

// XUnoTunnel
sal_Int64 SAL_CALL ImageWrapper::getSomething( const Sequence< sal_Int8 >& aIdentifier ) throw ( RuntimeException )
{
    if ( aIdentifier == impl_getStaticIdentifier() )
        return (sal_Int64)this;
    else
        return 0;
}

// XTypeProvider
Sequence< Type > SAL_CALL ImageWrapper::getTypes() throw ( RuntimeException )
{
    // Optimize this method !
    // We initialize a static variable only one time. And we don't must use a mutex at every call!
    // For the first call; pTypeCollection is NULL - for the second call pTypeCollection is different from NULL!
    static ::cppu::OTypeCollection* pTypeCollection = NULL ;

    if ( pTypeCollection == NULL )
    {
        // Ready for multithreading; get global mutex for first call of this method only! see before
        osl::MutexGuard aGuard( osl::Mutex::getGlobalMutex() ) ;

        // Control these pointer again ... it can be, that another instance will be faster then these!
        if ( pTypeCollection == NULL )
        {
            // Create a static typecollection ...
            static ::cppu::OTypeCollection aTypeCollection(
                        ::getCppuType(( const Reference< XTypeProvider                  >*)NULL ) ,
                        ::getCppuType(( const Reference< XUnoTunnel                     >*)NULL ) ,
                        ::getCppuType(( const Reference< com::sun::star::awt::XBitmap   >*)NULL )   ) ;

            // ... and set his address to static pointer!
            pTypeCollection = &aTypeCollection ;
        }
    }

    return pTypeCollection->getTypes() ;
}

Sequence< sal_Int8 > SAL_CALL ImageWrapper::getImplementationId() throw ( RuntimeException )
{
    // Create one Id for all instances of this class.
    // Use ethernet address to do this! (sal_True)

    // Optimize this method
    // We initialize a static variable only one time. And we don't must use a mutex at every call!
    // For the first call; pID is NULL - for the second call pID is different from NULL!
    static ::cppu::OImplementationId* pID = NULL ;

    if ( pID == NULL )
    {
        // Ready for multithreading; get global mutex for first call of this method only! see before
        osl::MutexGuard aGuard( osl::Mutex::getGlobalMutex() ) ;

        // Control these pointer again ... it can be, that another instance will be faster then these!
        if ( pID == NULL )
        {
            // Create a new static ID ...
            static ::cppu::OImplementationId aID( sal_False ) ;
            // ... and set his address to static pointer!
            pID = &aID ;
        }
    }

    return pID->getImplementationId() ;
}

}
