/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: CDriver.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 06:59:33 $
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

#ifndef _CONNECTIVITY_CALC_DRIVER_HXX_
#define _CONNECTIVITY_CALC_DRIVER_HXX_

#ifndef _CONNECTIVITY_FILE_ODRIVER_HXX_
#include "file/FDriver.hxx"
#endif

namespace connectivity
{
    namespace calc
    {
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
            ODriver_CreateInstance(const ::com::sun::star::uno::Reference<
                        ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory)
                    throw( ::com::sun::star::uno::Exception );

        class ODriver : public file::OFileDriver
        {
        public:
            ODriver(const ::com::sun::star::uno::Reference<
                                ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory) :
                file::OFileDriver(_rxFactory){}

            static ::rtl::OUString getImplementationName_Static(  ) throw(::com::sun::star::uno::RuntimeException);
            ::rtl::OUString SAL_CALL getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException);

            // XDriver
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL
                    connect( const ::rtl::OUString& url, const ::com::sun::star::uno::Sequence<
                                ::com::sun::star::beans::PropertyValue >& info )
                        throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual sal_Bool SAL_CALL acceptsURL( const ::rtl::OUString& url )
                        throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::uno::Sequence< ::com::sun::star::sdbc::DriverPropertyInfo > SAL_CALL getPropertyInfo( const ::rtl::OUString& url, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& info ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        };
    }
}

#endif //_CONNECTIVITY_CALC_DRIVER_HXX_

