/*************************************************************************
 *
 *  $RCSfile: confsvccomponent.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: jb $ $Date: 2002-12-06 13:08:26 $
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

#include "confsvccomponent.hxx"

#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif

#ifndef _CPPUHELPER_TYPEPROVIDER_HXX_
#include <cppuhelper/typeprovider.hxx>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#include <map>

namespace configmgr {


    ServiceComponentImpl::ServiceComponentImpl(ServiceImplementationInfo const* aInfo)
        : ServiceImplBase(m_aMutex)
        , m_aMutex()
        , m_info(aInfo)
    {
    }

    void ServiceComponentImpl::disposing()
    {
        ServiceImplBase::disposing();
    }
    void ServiceComponentImpl::checkAlive() throw (uno::RuntimeException)
    {
        checkAlive("Object was disposed");
    }
    void ServiceComponentImpl::checkAlive(OUString const& sMessage) throw (uno::RuntimeException)
    {
        if (rBHelper.bDisposed)
            throw lang::DisposedException(sMessage, *this);
    }

    // XTypeProvider
    uno::Sequence<sal_Int8> ServiceComponentImpl::getStaticImplementationId(ServiceImplementationInfo const* pServiceInfo)
        throw(uno::RuntimeException)
    {
        static osl::Mutex aMapMutex;
        static std::map<ServiceImplementationInfo const*, ::cppu::OImplementationId> aIdMap;

        osl::MutexGuard aMapGuard(aMapMutex);
        return aIdMap[pServiceInfo].getImplementationId();
    }

    uno::Sequence<sal_Int8> SAL_CALL ServiceComponentImpl::getImplementationId()
        throw(uno::RuntimeException)
    {
        return getStaticImplementationId(m_info);
    }

    // XServiceInfo
    OUString SAL_CALL ServiceComponentImpl::getImplementationName(  ) throw(uno::RuntimeException)
    {
        return ServiceInfoHelper(m_info).getImplementationName();
    }

    sal_Bool SAL_CALL ServiceComponentImpl::supportsService( const ::rtl::OUString& ServiceName ) throw(uno::RuntimeException)
    {
        return ServiceInfoHelper(m_info).supportsService( ServiceName );
    }

    uno::Sequence< OUString > SAL_CALL ServiceComponentImpl::getSupportedServiceNames(  ) throw(uno::RuntimeException)
    {
        return ServiceInfoHelper(m_info).getSupportedServiceNames( );
    }

    //ServiceComponentImpl::

} // namespace configmgr


