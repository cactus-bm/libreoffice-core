/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: localsinglebackend.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 04:06:32 $
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

#ifndef CONFIGMGR_LOCALBE_LOCALSINGLEBACKEND_HXX_
#define CONFIGMGR_LOCALBE_LOCALSINGLEBACKEND_HXX_

#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XSCHEMASUPPLIER_HPP_
#include <com/sun/star/configuration/backend/XSchemaSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XMULTILAYERSTRATUM_HPP_
#include <com/sun/star/configuration/backend/XMultiLayerStratum.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XBACKENDENTITIES_HPP_
#include <com/sun/star/configuration/backend/XBackendEntities.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_XCOMPONENTCONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XINITIALIZATION_HPP_
#include <com/sun/star/lang/XInitialization.hpp>
#endif // _COM_SUN_STAR_LANG_XINITIALIZATION_HPP_

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif // _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif // _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_

#ifndef _COM_SUN_STAR_CONFIGURATION_INVALIDBOOTSTRAPFILEEXCEPTION_HPP_
#include <com/sun/star/configuration/InvalidBootstrapFileException.hpp>
#endif

#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_CANNOTCONNECTEXCEPTION_HPP_
#include <com/sun/star/configuration/backend/CannotConnectException.hpp>
#endif

#ifndef _CPPUHELPER_COMPBASE5_HXX_
#include <cppuhelper/compbase5.hxx>
#endif // _CPPUHELPER_COMPBASE5_HXX_

namespace configmgr { namespace localbe {

namespace css = com::sun::star ;
namespace uno = css::uno ;
namespace lang = css::lang ;
namespace backend = css::configuration::backend ;

typedef cppu::WeakComponentImplHelper5<backend::XSchemaSupplier,
                                       backend::XMultiLayerStratum,
                                       backend::XBackendEntities,
                                       lang::XInitialization,
                                       lang::XServiceInfo> SingleBackendBase ;

/**
  Implements the SingleBackend service for local file access.
  Layer identifiers in that backend are file URLs.
  */
class LocalSingleBackend : public SingleBackendBase {
    public :
        /**
          Service constructor from a service factory.

          @param xFactory   service factory
          */
        LocalSingleBackend(const uno::Reference<uno::XComponentContext>& xContext) ;

        /** Destructor */
        ~LocalSingleBackend(void) ;


        // XInitialize
        virtual void SAL_CALL
            initialize( const uno::Sequence<uno::Any>& aParameters)
                throw (uno::RuntimeException, uno::Exception,
                       css::configuration::InvalidBootstrapFileException,
                       backend::CannotConnectException,
                       backend::BackendSetupException);

        // XSchemaSupplier
        virtual uno::Reference<backend::XSchema> SAL_CALL
            getComponentSchema( const rtl::OUString& aComponent )
                throw (backend::BackendAccessException,
                        lang::IllegalArgumentException,
                        uno::RuntimeException) ;

        // XMultiLayerStratum
        virtual uno::Sequence<rtl::OUString> SAL_CALL
            listLayerIds( const rtl::OUString& aComponent, const rtl::OUString& aEntity )
                throw (backend::BackendAccessException,
                        lang::IllegalArgumentException,
                        uno::RuntimeException) ;

        virtual rtl::OUString SAL_CALL
            getUpdateLayerId( const rtl::OUString& aComponent, const rtl::OUString& aEntity )
                throw (backend::BackendAccessException,
                        lang::IllegalArgumentException,
                        uno::RuntimeException) ;

        virtual uno::Reference<backend::XLayer> SAL_CALL
            getLayer( const rtl::OUString& aLayerId, const rtl::OUString& aTimestamp )
                throw (backend::BackendAccessException,
                        lang::IllegalArgumentException,
                        uno::RuntimeException) ;

        virtual uno::Sequence<uno::Reference<backend::XLayer> > SAL_CALL
            getLayers(const uno::Sequence<rtl::OUString>& aLayerIds,
                      const rtl::OUString& aTimestamp)
                throw (backend::BackendAccessException,
                        lang::IllegalArgumentException,
                        uno::RuntimeException) ;

        virtual uno::Sequence<uno::Reference<backend::XLayer> > SAL_CALL
            getMultipleLayers(const uno::Sequence<rtl::OUString>& aLayerIds,
                              const uno::Sequence<rtl::OUString>& aTimestamps)
                throw (backend::BackendAccessException,
                        lang::IllegalArgumentException,
                        uno::RuntimeException) ;

        virtual uno::Reference<backend::XUpdatableLayer> SAL_CALL
            getUpdatableLayer( const rtl::OUString& aLayerId )
                throw (backend::BackendAccessException,
                        lang::IllegalArgumentException,
                        uno::RuntimeException) ;

        // XBackendEntities
        virtual rtl::OUString SAL_CALL
            getOwnerEntity(  )
                throw (uno::RuntimeException);

        virtual rtl::OUString SAL_CALL
            getAdminEntity(  )
                throw (uno::RuntimeException);

        virtual sal_Bool SAL_CALL
            supportsEntity( const rtl::OUString& aEntity )
                throw (backend::BackendAccessException, uno::RuntimeException);

        virtual sal_Bool SAL_CALL
            isEqualEntity( const rtl::OUString& aEntity, const rtl::OUString& aOtherEntity )
                throw (backend::BackendAccessException, lang::IllegalArgumentException, uno::RuntimeException);

        // XServiceInfo
        virtual rtl::OUString SAL_CALL
            getImplementationName(  )
                throw (uno::RuntimeException) ;

        virtual sal_Bool SAL_CALL
            supportsService( const rtl::OUString& aServiceName )
                throw (uno::RuntimeException) ;

        virtual uno::Sequence<rtl::OUString> SAL_CALL
            getSupportedServiceNames(  )
                throw (uno::RuntimeException) ;

        /**
          Provides the implementation name.

          @return   implementation name
          */
        static rtl::OUString SAL_CALL getName(void) ;
        /**
          Provides the supported services names

          @return   service names
          */
        static uno::Sequence<rtl::OUString> SAL_CALL getServices(void) ;

    public: // helpers for other implementation that need to use the same data
        /**
          Locates the main layer data and localized data directories in a layer directory hierarchy
          */
        static bool getLayerSubDirectories( rtl::OUString const & aLayerBaseUrl,
                                             rtl::OUString& aMainLayerUrl,
                                             rtl::OUString& aSubLayerUrl);
        /**
          Creates a simple readonly non-composite layer for a component in a base directory
          */
        static uno::Reference<backend::XLayer>
            createSimpleLayer(const uno::Reference<lang::XMultiServiceFactory>& xFactory,
                              rtl::OUString const & aLayerBaseUrl,
                              rtl::OUString const & aComponent);

        /**
          Creates a simple readonly non-composite layer for a component in a given file
          */
        static uno::Reference<backend::XLayer>
            createSimpleLayer(const uno::Reference<lang::XMultiServiceFactory>& xFactory,
                              rtl::OUString const & aComponentUrl);
    private :
        /** Service factory */
        uno::Reference<lang::XMultiServiceFactory> mFactory ;
        /** Mutex for resources protection */
        osl::Mutex mMutex ;
        /**
          Base of the schema data. Is a list to allow
          for multiple schema directories.
          */
        uno::Sequence<rtl::OUString> mSchemaDataUrls ;
        /**
          Base of the default data. Is a list to allow
          for multiple layers of default data.
          */
        uno::Sequence<rtl::OUString> mDefaultDataUrls ;
        /** Base of the user data */
        rtl::OUString mUserDataUrl ;

        /** special index for entity */
        sal_Int32 findEntity(rtl::OUString const & _aEntity);
        /** parse and translate layer-id */
        sal_Int32 resolveLayerId(rtl::OUString const & _aLayerId, rtl::OUString & _aFile);

        /**
          Builds a LocalFileLayer object given a layer id.
          Since the LocalFileLayer implements the various
          interfaces a layer can be accessed as, a few methods
          need one. This method handles the layer id mapping
          and the existence or not of sublayers.

          @param aLayerId   layer id
          @return   local file layer
          @throws com::sun::star::lang::IllegalArgumentException
                  if the layer id is invalid.
          */
        uno::Reference<backend::XUpdatableLayer> getFileLayer(const rtl::OUString& aLayerId)
            throw (lang::IllegalArgumentException) ;
        /**
          Same as above, but using a component URL and layer index
          combination instead of a layer id (which encodes both).

          @param aComponent     component URL
          @param aLayerIndex    layer index
          @return   local file layer
          */
        uno::Reference<backend::XUpdatableLayer> getFileLayer(const rtl::OUString& aComponent,
                                                                sal_Int32 aLayerIndex) ;
        /**
          Maps a layer index (-1 for user layer, 0-x for defaults)
          to the appropriate layer and sublayers base directories.

          @param aLayerIndex    layer index
          @param aLayerUrl      layer base URL, filled on return
          @param aSubLayerUrl   sublayer base URL, filled on return
          */
        bool getLayerDirectories(sal_Int32 aLayerIndex,
                                 rtl::OUString& aLayerUrl,
                                 rtl::OUString& aSubLayerUrl) ;
        /**
          Tells if a file is more recent than a given date.
          The date is formatted YYYYMMDDhhmmssZ.

          @param aComponent     URL of the component to check
          @param aLayerIndex    index of the layer involved (-1 = user)
          @param aTimestamp     timestamp to check against
          @return   sal_True if the file is more recent, sal_False otherwise
          */
        sal_Bool isMoreRecent(const rtl::OUString& aComponent,
                              sal_Int32 aLayerId,
                              const rtl::OUString& aTimestamp) ;
    } ;

} } // configmgr.localbe

#endif // CONFIGMGR_LOCALBE_LOCALSINGLEBACKEND_HXX_
