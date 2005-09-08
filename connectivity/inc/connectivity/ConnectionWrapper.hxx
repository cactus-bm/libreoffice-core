/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ConnectionWrapper.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 04:56:41 $
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

#ifndef _CONNECTIVITY_ZCONNECTIONWRAPPER_HXX_
#define _CONNECTIVITY_ZCONNECTIONWRAPPER_HXX_

#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCONNECTION_HPP_
#include <com/sun/star/sdbc/XConnection.hpp>
#endif
#ifndef _COMPHELPER_BROADCASTHELPER_HXX_
#include <comphelper/broadcasthelper.hxx>
#endif
#ifndef _CONNECTIVITY_COMMONTOOLS_HXX_
#include "connectivity/CommonTools.hxx"
#endif

namespace connectivity
{

    //==========================================================================
    //= OConnectionWrapper - wraps all methods to the real connection from the driver
    //= but when disposed it doesn't dispose the real connection
    //==========================================================================
    typedef ::cppu::ImplHelper2<        ::com::sun::star::lang::XServiceInfo,
                                        ::com::sun::star::lang::XUnoTunnel
                                > OConnection_BASE;

    class OConnectionWrapper :   public OConnection_BASE
    {
    protected:
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation > m_xProxyConnection;
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > m_xConnection;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XTypeProvider > m_xTypeProvider;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XUnoTunnel > m_xUnoTunnel;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XServiceInfo > m_xServiceInfo;

        virtual ~OConnectionWrapper();
        void setDelegation(::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation >& _rxProxyConnection,oslInterlockedCount& _rRefCount);
        void setDelegation(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _xConnection
            ,const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _xORB
            ,oslInterlockedCount& _rRefCount);
        // must be called from derived classes
        void disposing();
    public:
        OConnectionWrapper( );

        // XServiceInfo
        DECLARE_SERVICE_INFO();
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& _rType ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw (::com::sun::star::uno::RuntimeException);

        // com::sun::star::lang::XUnoTunnel
        virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);
        static ::com::sun::star::uno::Sequence< sal_Int8 > getUnoTunnelImplementationId();
        /** method to create unique ids
            @param  _rURL
                The URL.
            @param  _rInfo
                The info property of the datasource. It will be resorted if needed.
            @param  _pBuffer
                Here we store the digest. Must not NULL.
            @param  _rUserName
                The user name.
            @param  _rPassword
                The password.
        */
        static void createUniqueId( const ::rtl::OUString& _rURL
                    ,::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& _rInfo
                    ,sal_uInt8* _pBuffer
                    ,const ::rtl::OUString& _rUserName = ::rtl::OUString()
                    ,const ::rtl::OUString& _rPassword = ::rtl::OUString());
    };
}
#endif // _CONNECTIVITY_ZCONNECTIONWRAPPER_HXX_

