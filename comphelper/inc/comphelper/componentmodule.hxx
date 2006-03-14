/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: componentmodule.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2006-03-14 11:39:27 $
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
#ifndef COMPHELPER_INC_COMPHELPER_COMPONENTMODULE_HXX
#define COMPHELPER_INC_COMPHELPER_COMPONENTMODULE_HXX

#ifndef INCLUDED_COMPHELPERDLLAPI_H
#include <comphelper/comphelperdllapi.h>
#endif

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_REGISTRY_XREGISTRYKEY_HPP_
#include <com/sun/star/registry/XRegistryKey.hpp>
#endif
/** === end UNO includes === **/

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef _CPPUHELPER_FACTORY_HXX_
#include <cppuhelper/factory.hxx>
#endif
#ifndef _RTL_STRING_HXX_
#include <rtl/string.hxx>
#endif

//........................................................................
namespace comphelper
{
//........................................................................

    /** factory factory declaration
    */
    typedef ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory > (SAL_CALL *FactoryInstantiation)
    (
        ::cppu::ComponentFactoryFunc _pFactoryFunc,
        ::rtl::OUString const& _rComponentName,
        ::com::sun::star::uno::Sequence< ::rtl::OUString > const & _rServiceNames,
        rtl_ModuleCount* _pModuleCounter
    ) SAL_THROW(());

    //=========================================================================
    //= ComponentDescription
    //=========================================================================
    struct COMPHELPER_DLLPUBLIC ComponentDescription
    {
        ::rtl::OUString                                     sImplementationName;
        ::com::sun::star::uno::Sequence< ::rtl::OUString >  aSupportedServices;
        ::cppu::ComponentFactoryFunc                        pComponentCreationFunc;
        FactoryInstantiation                                pFactoryCreationFunc;

        ComponentDescription()
            :sImplementationName()
            ,aSupportedServices()
            ,pComponentCreationFunc( NULL )
            ,pFactoryCreationFunc( NULL )
        {
        }

        ComponentDescription( const ::rtl::OUString& _rImplementationName, const ::com::sun::star::uno::Sequence< ::rtl::OUString >& _rSupportedServices,
                ::cppu::ComponentFactoryFunc _pComponentCreationFunc, FactoryInstantiation _pFactoryCreationFunc )
            :sImplementationName( _rImplementationName )
            ,aSupportedServices( _rSupportedServices )
            ,pComponentCreationFunc( _pComponentCreationFunc )
            ,pFactoryCreationFunc( _pFactoryCreationFunc )
        {
        }
    };

    //=========================================================================
    //= OModule
    //=========================================================================
    class OModuleImpl;
    class COMPHELPER_DLLPUBLIC OModule
    {
    private:
        oslInterlockedCount     m_nClients;     /// number of registered clients
        OModuleImpl*            m_pImpl;        /// impl class. lives as long as at least one client for the module is registered

    protected:
        mutable ::osl::Mutex    m_aMutex;       /// access safety

    public:
        OModule();

        /** register a component implementing a service with the given data.
            @param _rImplementationName
                the implementation name of the component
            @param _rServiceNames
                the services the component supports
            @param _pCreateFunction
                a function for creating an instance of the component
            @param _pFactoryFunction
                a function for creating a factory for that component
        */
        void registerImplementation(
            const ::rtl::OUString& _rImplementationName,
            const ::com::sun::star::uno::Sequence< ::rtl::OUString >& _rServiceNames,
            ::cppu::ComponentFactoryFunc _pCreateFunction,
            FactoryInstantiation _pFactoryFunction = ::cppu::createSingleComponentFactory );

        /** registers a component given by <type>ComponentDescription</type>
        */
        void registerImplementation( const ComponentDescription& _rComp );

        /** write the registration information of all known components
            <p>writes the registration information of all components which are currently registered into the
            specified registry.<p/>
            <p>Usually used from within component_writeInfo.<p/>
            @param_rxServiceManager
                the service manager
            @param _rRootKey
                the registry key under which the information will be stored
            @return
                <TRUE/> if the registration of all implementations was successfull, <FALSE/> otherwise
        */
        sal_Bool writeComponentInfos(
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxServiceManager,
            const ::com::sun::star::uno::Reference< ::com::sun::star::registry::XRegistryKey >& _rRootKey);

        /** creates a Factory for the component with the given implementation name.
            <p>Usually used from within component_getFactory.<p/>
            @param _rxServiceManager
                a pointer to an XMultiServiceFactory interface as got in component_getFactory
            @param _pImplementationName
                the implementation name of the component
            @return
                the XInterface access to a factory for the component
        */
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > getComponentFactory(
            const ::rtl::OUString& _rImplementationName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxServiceManager
        );

    public:
        class ClientAccess { friend class OModuleClient; private: ClientAccess() { } };
        /// register a client for the module
        void registerClient( ClientAccess );
        /// revoke a client for the module
        void revokeClient( ClientAccess );

    protected:
        /** called when the first client has been registered
            @precond
                <member>m_aMutex</member> is locked
        */
        virtual void onFirstClient();

        /** called when the last client has been revoked
            @precond
                <member>m_aMutex</member> is locked
        */
        virtual void onLastClient();

    private:
        OModule( const OModule& );              // never implemented
        OModule& operator=( const OModule& );   // never implemented
    };

    //=========================================================================
    //= OModuleClient
    //=========================================================================
    /** base class for objects which uses any global module-specific ressources
    */
    class COMPHELPER_DLLPUBLIC OModuleClient
    {
    protected:
        OModule&    m_rModule;

    public:
        OModuleClient( OModule& _rModule ) :m_rModule( _rModule )   { m_rModule.registerClient( OModule::ClientAccess() ); }
        ~OModuleClient()                                            { m_rModule.revokeClient( OModule::ClientAccess() ); }
    };

    //==========================================================================
    //= OAutoRegistration
    //==========================================================================
    template <class TYPE>
    class OAutoRegistration
    {
    public:
        /** automatically provides all component information to an OModule instance
            <p>Assumed that the template argument has the three methods
                <ul>
                    <li><code>static ::rtl::OUString getImplementationName_static()</code><li/>
                    <li><code>static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_static()</code><li/>
                    <li><code>static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
                        Create(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&)</code>
                        </li>
                <ul/>
            the instantiation of this object will automatically register the class via <method>OModule::registerImplementation</method>.
            <p/>
            The factory creation function used is <code>::cppu::createSingleComponentFactory</code>.
            @see OOneInstanceAutoRegistration
        */
        OAutoRegistration( OModule& _rModule );
    };

    template <class TYPE>
    OAutoRegistration<TYPE>::OAutoRegistration( OModule& _rModule )
    {
        _rModule.registerImplementation(
            TYPE::getImplementationName_static(),
            TYPE::getSupportedServiceNames_static(),
            TYPE::Create
        );
    }

//........................................................................
} // namespace comphelper
//........................................................................

#endif // COMPHELPER_INC_COMPHELPER_COMPONENTMODULE_HXX

