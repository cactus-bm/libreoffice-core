/*************************************************************************
 *
 *  $RCSfile: providerfactory.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: jb $ $Date: 2002-09-19 10:52:06 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
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

#ifndef _CONFIGMGR_PROVIDER_FACTORY_HXX_
#define _CONFIGMGR_PROVIDER_FACTORY_HXX_

#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_XSINGLECOMPONENTFACTORY_HPP_
#include <com/sun/star/lang/XSingleComponentFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_ILLEGALARGUMENTEXCEPTION_HPP_
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif

#include "confapifactory.hxx"

#ifndef _COM_SUN_STAR_LANG_XEVENTLISTENER_HPP_
#include <com/sun/star/lang/XEventListener.hpp>
#endif

//........................................................................
namespace configmgr
{
//........................................................................
    namespace uno  = ::com::sun::star::uno;
    namespace lang = ::com::sun::star::lang;
    using ::rtl::OUString;
//........................................................................

    uno::Reference< lang::XSingleServiceFactory > SAL_CALL
        createProviderFactory(
            const uno::Reference< lang::XMultiServiceFactory >& _rServiceManager,
            const OUString& _rComponentName,
            ::configmgr::ProviderInstantiation _pCreateFunction,
            const uno::Sequence< OUString >& _rServiceNames
        );

    class ConnectionSettings;
    class BootstrapSettings;

    //====================================================================
    //= OProviderFactory
    //====================================================================
    typedef ::cppu::WeakImplHelper2< lang::XSingleServiceFactory, lang::XSingleComponentFactory > OProviderFactory_Base;
    /** a special factory for the configuration provider, which implements some kind of
        "shared multiple instances" factory.
    */

    class OProviderFactory : public OProviderFactory_Base
    {
        friend class ODisposingListener;
    protected:
        ::osl::Mutex                        m_aMutex;
        ProviderInstantiation               m_pObjectCreator;

        uno::Reference< lang::XEventListener >          m_xEventListener; // must be the first uno::object
        uno::Reference< lang::XMultiServiceFactory >    m_xORB;
        uno::Reference< uno::XInterface >               m_xDefaultProvider;

        // the pure settings, not overwritten by any runtime arguments
        BootstrapSettings const*            m_pPureSettings;

        typedef uno::WeakReference< uno::XInterface >   ProviderReference;
        DECLARE_STL_USTRINGACCESS_MAP(ProviderReference, ProviderCache);
        ProviderCache   m_aProviders;

        typedef uno::Reference< uno::XComponentContext > Context;
    public:
        OProviderFactory(
            const uno::Reference< lang::XMultiServiceFactory >& _rxORB,
            ProviderInstantiation _pObjectCreator);
        ~OProviderFactory();

        virtual uno::Reference< uno::XInterface > SAL_CALL createInstance(  ) throw(uno::Exception, uno::RuntimeException);
        virtual uno::Reference< uno::XInterface > SAL_CALL createInstanceWithArguments( const uno::Sequence< uno::Any >& aArguments ) throw(uno::Exception, uno::RuntimeException);

        virtual uno::Reference< uno::XInterface >
            SAL_CALL createInstanceWithContext( const uno::Reference< uno::XComponentContext >& xContext )
                throw (uno::Exception, ::com::sun::star::uno::RuntimeException);

        virtual uno::Reference< uno::XInterface > SAL_CALL
            createInstanceWithArgumentsAndContext( const uno::Sequence< uno::Any >& aArguments, const uno::Reference< uno::XComponentContext >& xContext )
                throw (uno::Exception, uno::RuntimeException);

        uno::Reference< uno::XInterface > createProvider(Context const & xContext);
        uno::Reference< uno::XInterface > createProviderWithArguments(Context const & xContext, const uno::Sequence< uno::Any >& _rArguments);
        uno::Reference< uno::XInterface > createProviderWithSettings(Context const & xContext, const ConnectionSettings& _rSettings);

    protected:
        void    ensureDefaultProvider(Context const & xContext);
        void    ensureBootstrapSettings(Context const & xContext);

        uno::Reference< uno::XInterface > implCreateProviderWithSettings(const ConnectionSettings& _rSettings, bool bRequiresBootstrap);
        // from the given map, extract a provider for the given user. (if necessary, create one and insert it into the map)
        uno::Reference< uno::XInterface > implGetProvider( const ConnectionSettings& _rSettings );

        // to be called with m:aMutex locked
        void disposing(com::sun::star::lang::EventObject const& rEvt) throw();
    };

//........................................................................
}   // namespace configmgr
//........................................................................

#endif // _CONFIGMGR_PROVIDER_FACTORY_HXX_

/*************************************************************************
 * history:
 *  $Log: not supported by cvs2svn $
 *  Revision 1.6  2001/06/22 08:26:18  jb
 *  Correct argument-dependent caching of providers
 *
 *  Revision 1.5  2001/05/18 16:16:52  jb
 *  #81412# Cleaned up bootstrap settings handling; Added recognition of bootstrap errors
 *
 *  Revision 1.4  2001/04/03 16:33:58  jb
 *  Local AdministrationProvider now mapped to Setup-session
 *
 *  Revision 1.3  2001/01/29 08:51:11  dg
 *  #82336# invalid syntax for template
 *
 *  Revision 1.2  2001/01/26 07:54:21  lla
 *  #82734# disposing with lasy writing necessary
 *
 *  Revision 1.1  2000/12/01 13:53:17  fs
 *  initial checkin - afctory for configuration provider(s)
 *
 *
 *  Revision 1.0 30.11.00 19:03:57  fs
 ************************************************************************/

