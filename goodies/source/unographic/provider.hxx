/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: provider.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: kz $ $Date: 2007-10-10 15:33:21 $
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

#ifndef _GOODIES_PROVIDER_HXX
#define _GOODIES_PROVIDER_HXX

#include <cppuhelper/implbase1.hxx>

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_GRAPHIC_XGRAPHICPROVIDER_HPP_
#include <com/sun/star/graphic/XGraphicProvider.hpp>
#endif

using namespace com::sun::star;

namespace unographic {

// -------------------
// - GraphicProvider -
// -------------------

class GraphicProvider : public ::cppu::WeakImplHelper1< ::com::sun::star::graphic::XGraphicProvider >
{
public:

    GraphicProvider();
    ~GraphicProvider();

    static ::rtl::OUString getImplementationName_Static() throw();
    static ::com::sun::star::uno::Sequence< ::rtl::OUString >  getSupportedServiceNames_Static() throw();

protected:

    // XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL supportsService( const rtl::OUString& ServiceName ) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames() throw( ::com::sun::star::uno::RuntimeException );

    // XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  ) throw(::com::sun::star::uno::RuntimeException);

    // XGraphicProvider
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL queryGraphicDescriptor( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& MediaProperties ) throw (::com::sun::star::io::IOException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > SAL_CALL queryGraphic( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& MediaProperties ) throw (::com::sun::star::io::IOException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL storeGraphic( const ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic >& Graphic, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& MediaProperties ) throw (::com::sun::star::io::IOException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

private:

    ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > implLoadMemory( const ::rtl::OUString& rResourceURL ) const;
    ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > implLoadResource( const ::rtl::OUString& rResourceURL ) const;
    ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > implLoadRepositoryImage( const ::rtl::OUString& rResourceURL ) const;
    ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > implLoadStandardImage( const ::rtl::OUString& rResourceURL ) const;
};

}

#endif
