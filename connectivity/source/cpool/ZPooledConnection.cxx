/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ZPooledConnection.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 05:18:16 $
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
#ifndef CONNECTIVITY_POOLEDCONNECTION_HXX
#include "ZPooledConnection.hxx"
#endif
#ifndef _CONNECTIVITY_ZCONNECTIONWEAKWRAPPER_HXX_
#include "ZConnectionWrapper.hxx"
#endif
#ifndef _CONNECTIVITY_CONNECTIONWRAPPER_HXX_
#include "connectivity/ConnectionWrapper.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBC_XCLOSEABLE_HPP_
#include <com/sun/star/sdbc/XCloseable.hpp>
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
#ifndef _CPPUHELPER_COMPONENT_HXX_
#include <cppuhelper/component.hxx>
#endif
#ifndef _CPPUHELPER_COMPBASE1_HXX_
#include <cppuhelper/compbase1.hxx>
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::reflection;
using namespace connectivity;
using namespace ::osl;

OPooledConnection::OPooledConnection(const Reference< XConnection >& _xConnection,
                                    const Reference< ::com::sun::star::reflection::XProxyFactory >& _rxProxyFactory)
    : OPooledConnection_Base(m_aMutex)
    ,m_xRealConnection(_xConnection)
    ,m_xProxyFactory(_rxProxyFactory)
{

}
// -----------------------------------------------------------------------------
// OComponentHelper
void SAL_CALL OPooledConnection::disposing(void)
{
    MutexGuard aGuard(m_aMutex);
    if (m_xComponent.is())
        m_xComponent->removeEventListener(this);
    m_xComponent = NULL;
    ::comphelper::disposeComponent(m_xRealConnection);
}
// -----------------------------------------------------------------------------
// XEventListener
void SAL_CALL OPooledConnection::disposing( const EventObject& Source ) throw (RuntimeException)
{
    m_xComponent = NULL;
}
// -----------------------------------------------------------------------------
//XPooledConnection
Reference< XConnection > OPooledConnection::getConnection()  throw(SQLException, RuntimeException)
{
    if(!m_xComponent.is() && m_xRealConnection.is())
    {
        Reference< XAggregation > xConProxy = m_xProxyFactory->createProxy(m_xRealConnection.get());
        m_xComponent = new OConnectionWeakWrapper(xConProxy);
        // register as event listener for the new connection
        if (m_xComponent.is())
            m_xComponent->addEventListener(this);
    }
    return Reference< XConnection >(m_xComponent,UNO_QUERY);
}
// -----------------------------------------------------------------------------

