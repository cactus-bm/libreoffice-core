/*************************************************************************
 *
 *  $RCSfile: confprovider2.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-15 13:34:46 $
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
#ifndef CONFIGMGR_API_CONFPROVIDER2_HXX_
#define CONFIGMGR_API_CONFPROVIDER2_HXX_

#ifndef CONFIGMGR_API_PROVIDER_HXX_
#include "provider.hxx"
#endif

#ifndef _COMPHELPER_PROPERTY_ARRAY_HELPER_HXX_
#include <comphelper/proparrhlp.hxx>
#endif


namespace configmgr
{
    namespace css  = ::com::sun::star;
    namespace uno  = css::uno;
    namespace lang = css::lang;
    using ::rtl::OUString;
    using ::vos::ORef;

    class OConfigurationProviderImpl;

    //==========================================================================
    //= OConfigurationProvider
    //==========================================================================
    /** Factory for receiving access for user configuration data. */
    class OConfigurationProvider : public OProvider
                                 , public  ::comphelper::OPropertyArrayUsageHelper<OConfigurationProvider>
    {
        OConfigurationProviderImpl*         m_pImpl;
        uno::Sequence< ::rtl::OUString >    m_aPrefetchNodes;
        sal_Bool                            m_bEnableAsync;

    public:
        OConfigurationProvider(CreationContext const & xContext, ServiceImplementationInfo const* pServices);
        void connect() throw (uno::Exception);

    public:
        ~OConfigurationProvider();

        /// XMultiServiceFactory
        virtual uno::Reference< uno::XInterface > SAL_CALL
            createInstance( const OUString& aServiceSpecifier )
                throw(uno::Exception, uno::RuntimeException);

        virtual uno::Reference< uno::XInterface > SAL_CALL
            createInstanceWithArguments( const ::rtl::OUString& ServiceSpecifier, const uno::Sequence< uno::Any >& Arguments )
                throw(uno::Exception, uno::RuntimeException);

        virtual uno::Sequence< OUString > SAL_CALL
            getAvailableServiceNames(  )
                throw(uno::RuntimeException);

        /// XLocalizable
        virtual void SAL_CALL
            setLocale( const lang::Locale& eLocale )
                throw (uno::RuntimeException);

        virtual lang::Locale SAL_CALL
            getLocale(  )
                throw (uno::RuntimeException);


        //XRefreshable
         virtual void SAL_CALL
            refresh(  )
                throw (uno::RuntimeException);
        virtual void SAL_CALL
            addRefreshListener(
                const uno::Reference< util::XRefreshListener >& aListener )
                    throw (uno::RuntimeException);
        virtual void SAL_CALL
            removeRefreshListener(
                const uno::Reference< util::XRefreshListener >& aListener )
                    throw (uno::RuntimeException);
        //XFlushable
         virtual void SAL_CALL
            flush(  )
                throw (uno::RuntimeException);
        virtual void SAL_CALL
            addFlushListener(
                const uno::Reference< util::XFlushListener >& aListener )
                    throw (uno::RuntimeException);
        virtual void SAL_CALL
            removeFlushListener(
                const uno::Reference< util::XFlushListener >& aListener )
                    throw (uno::RuntimeException);

        // OPropertSetHelper
        virtual void SAL_CALL setFastPropertyValue_NoBroadcast(
                                sal_Int32 nHandle,
                                const ::com::sun::star::uno::Any& rValue
                                                 )
                                                 throw (::com::sun::star::uno::Exception);

        // XInterface
        virtual uno::Any SAL_CALL queryInterface( const uno::Type & rType ) throw(uno::RuntimeException);
        //XTypeProvider
        virtual uno::Sequence< uno::Type > SAL_CALL getTypes(  ) throw(uno::RuntimeException);

    protected:
        virtual void SAL_CALL disposing();

        // OPropertyArrayUsageHelper
        virtual ::cppu::IPropertyArrayHelper* createArrayHelper( ) const;

        // OPropertySetHelper
        virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();
    };


} // namespace configmgr

#endif // CONFIGMGR_API_CONFPROVIDER2_HXX_


