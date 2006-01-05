/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: moduleimagemanager.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: kz $ $Date: 2006-01-05 18:10:04 $
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

#ifndef __FRAMEWORK_UICONFIGURATION_MODULEIMAGEMANAGER_HXX_
#define __FRAMEWORK_UICONFIGURATION_MODULEIMAGEMANAGER_HXX_


/** Attention: stl headers must(!) be included at first. Otherwhise it can make trouble
               with solaris headers ...
*/
#include <vector>
#include <list>
#include <hash_map>

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
#include <threadhelp/threadhelpbase.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_GENERIC_HXX_
#include <macros/generic.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_XINTERFACE_HXX_
#include <macros/xinterface.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_XTYPEPROVIDER_HXX_
#include <macros/xtypeprovider.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_XSERVICEINFO_HXX_
#include <macros/xserviceinfo.hxx>
#endif

#ifndef __FRAMEWORK_STDTYPES_H_
#include <stdtypes.h>
#endif

#ifndef __FRAMEWORK_UICONFIGURATION_IMAGETYPE_HXX_
#include <uiconfiguration/imagetype.hxx>
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XTYPEPROVIDER_HPP_
#include <com/sun/star/lang/XTypeProvider.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_XUICONFIGURATIONPERSISTENCE_HPP_
#include <com/sun/star/ui/XUIConfigurationPersistence.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_XUICONFIGURATIONSTORAGE_HPP_
#include <com/sun/star/ui/XUIConfigurationStorage.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_XUICONFIGURATION_HPP_
#include <com/sun/star/ui/XUIConfiguration.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_XIMAGEMANAGER_HPP_
#include <com/sun/star/ui/XImageManager.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XINITIALIZATION_HPP_
#include <com/sun/star/lang/XInitialization.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_CONFIGURATIONEVENT_HPP_
#include <com/sun/star/ui/ConfigurationEvent.hpp>
#endif

#ifndef _COM_SUN_STAR_EMBED_XTRANSACTEDOBJECT_HPP_
#include <com/sun/star/embed/XTransactedObject.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_XIMAGEMANAGER_HPP_
#include <com/sun/star/ui/XImageManager.hpp>
#endif

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

#ifndef _CPPUHELPER_INTERFACECONTAINER_HXX_
#include <cppuhelper/interfacecontainer.hxx>
#endif

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#include <vcl/image.hxx>
#include <tools/color.hxx>
#include <rtl/ref.hxx>

#include <vector>

namespace framework
{
    class CmdImageList
    {
        public:
            CmdImageList( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& rServiceManager,
                          const ::rtl::OUString& aModuleIdentifier );
            virtual ~CmdImageList();

            virtual Image                           getImageFromCommandURL( sal_Int16 nImageType, const rtl::OUString& rCommandURL );
            virtual bool                            hasImage( sal_Int16 nImageType, const rtl::OUString& rCommandURL );
            virtual ::std::vector< rtl::OUString >& getImageNames();
            virtual ::std::vector< rtl::OUString >& getImageCommandNames();

        protected:
            void                            impl_fillCommandToImageNameMap();
            ImageList*                      impl_getImageList( sal_Int16 nImageType );
            std::vector< ::rtl::OUString >& impl_getImageNameVector();
            std::vector< ::rtl::OUString >& impl_getImageCommandNameVector();

        private:
            sal_Bool                                                                         m_bVectorInit;
            rtl::OUString                                                                    m_aModuleIdentifier;
            ImageList*                                                                       m_pImageList[ImageType_COUNT];
            CommandToImageNameMap                                                            m_aCommandToImageNameMap;
            ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xServiceManager;
            ::std::vector< rtl::OUString >                                                   m_aImageNameVector;
            ::std::vector< rtl::OUString >                                                   m_aImageCommandNameVector;
            sal_Int16                                                                        m_nSymbolsStyle;
    };

    class GlobalImageList : public CmdImageList, public rtl::IReference
    {
        public:
            GlobalImageList( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& rServiceManager );
            virtual ~GlobalImageList();

            virtual Image                           getImageFromCommandURL( sal_Int16 nImageType, const rtl::OUString& rCommandURL );
            virtual bool                            hasImage( sal_Int16 nImageType, const rtl::OUString& rCommandURL );
            virtual ::std::vector< rtl::OUString >& getImageNames();
            virtual ::std::vector< rtl::OUString >& getImageCommandNames();

            // �Reference
            virtual oslInterlockedCount SAL_CALL acquire();
            virtual oslInterlockedCount SAL_CALL release();

        private:
            oslInterlockedCount                                                              m_nRefCount;
    };

    class ModuleImageManager :    public com::sun::star::lang::XTypeProvider                      ,
                                  public ::com::sun::star::ui::XImageManager                ,
                                  private ThreadHelpBase                                          , // Struct for right initalization of mutex member! Must be first of baseclasses.
                                  public ::cppu::OWeakObject
    {
        public:
            //  XInterface, XTypeProvider, XServiceInfo
            DECLARE_XINTERFACE
            DECLARE_XTYPEPROVIDER

            ModuleImageManager( com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory > xServiceManager );
            virtual ~ModuleImageManager();

            // XComponent
            virtual void SAL_CALL dispose() throw (::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener ) throw (::com::sun::star::uno::RuntimeException);

            // XInitialization
            virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

            // XImageManager
            virtual void SAL_CALL reset() throw (::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getAllImageNames( ::sal_Int16 nImageType ) throw (::com::sun::star::uno::RuntimeException);
            virtual ::sal_Bool SAL_CALL hasImage( ::sal_Int16 nImageType, const ::rtl::OUString& aCommandURL ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > > SAL_CALL getImages( ::sal_Int16 nImageType, const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aCommandURLSequence ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL replaceImages( ::sal_Int16 nImageType, const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aCommandURLSequence, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > >& aGraphicsSequence ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IllegalAccessException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL removeImages( ::sal_Int16 nImageType, const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aResourceURLSequence ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IllegalAccessException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL insertImages( ::sal_Int16 nImageType, const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aCommandURLSequence, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > >& aGraphicSequence ) throw (::com::sun::star::container::ElementExistException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IllegalAccessException, ::com::sun::star::uno::RuntimeException);

            // XUIConfiguration
            virtual void SAL_CALL addConfigurationListener( const ::com::sun::star::uno::Reference< ::com::sun::star::ui::XUIConfigurationListener >& Listener ) throw (::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL removeConfigurationListener( const ::com::sun::star::uno::Reference< ::com::sun::star::ui::XUIConfigurationListener >& Listener ) throw (::com::sun::star::uno::RuntimeException);

            // XUIConfigurationPersistence
            virtual void SAL_CALL reload() throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL store() throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL storeToStorage( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& Storage ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
            virtual sal_Bool SAL_CALL isModified() throw (::com::sun::star::uno::RuntimeException);
            virtual sal_Bool SAL_CALL isReadOnly() throw (::com::sun::star::uno::RuntimeException);

        private:
            typedef std::hash_map< rtl::OUString,
                                   sal_Bool,
                                   OUStringHashCode,
                                   ::std::equal_to< ::rtl::OUString > > ImageNameMap;

            // private data types
            enum Layer
            {
                LAYER_DEFAULT,
                LAYER_USERDEFINED,
                LAYER_COUNT
            };

            enum NotifyOp
            {
                NotifyOp_Remove,
                NotifyOp_Insert,
                NotifyOp_Replace
            };

            typedef ::std::vector< ::com::sun::star::ui::ConfigurationEvent > ConfigEventNotifyContainer;

            // private methods
            void                                      implts_initialize();
            void                                      implts_notifyContainerListener( const ::com::sun::star::ui::ConfigurationEvent& aEvent, NotifyOp eOp );
            const rtl::Reference< GlobalImageList >&  implts_getGlobalImageList();
            CmdImageList*                             implts_getDefaultImageList();
            ImageList*                                implts_getUserImageList( ImageType nImageType );
            sal_Bool                                  implts_loadUserImages( ImageType nImageType,
                                                                             const com::sun::star::uno::Reference< com::sun::star::embed::XStorage >& xUserImageStorage,
                                                                             const com::sun::star::uno::Reference< com::sun::star::embed::XStorage >& xUserBitmapsStorage );
            sal_Bool                                  implts_storeUserImages( ImageType nImageType,
                                                                              const com::sun::star::uno::Reference< com::sun::star::embed::XStorage >& xUserImageStorage,
                                                                              const com::sun::star::uno::Reference< com::sun::star::embed::XStorage >& xUserBitmapsStorage );

            com::sun::star::uno::Reference< com::sun::star::embed::XStorage >               m_xUserConfigStorage;
            com::sun::star::uno::Reference< com::sun::star::embed::XStorage >               m_xUserImageStorage;
            com::sun::star::uno::Reference< com::sun::star::embed::XStorage >               m_xUserBitmapsStorage;
            bool                                                                            m_bReadOnly;
            bool                                                                            m_bInitialized;
            bool                                                                            m_bModified;
            bool                                                                            m_bConfigRead;
            bool                                                                            m_bDisposed;
            rtl::OUString                                                                   m_aXMLPostfix;
            rtl::OUString                                                                   m_aModuleIdentifier;
            rtl::OUString                                                                   m_aResourceString;
            com::sun::star::uno::Reference< com::sun::star::embed::XTransactedObject >      m_xUserRootCommit;
            com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >    m_xServiceManager;
            ::cppu::OMultiTypeInterfaceContainerHelper                                      m_aListenerContainer;   /// container for ALL Listener
            rtl::Reference< GlobalImageList >                                               m_pGlobalImageList;
            CmdImageList*                                                                   m_pDefaultImageList;
            ImageList*                                                                      m_pUserImageList[ImageType_COUNT];
            bool                                                                            m_bUserImageListModified[ImageType_COUNT];
   };
}

#endif // __FRAMEWORK_UICONFIGURATION_MODULEUICONFIGMANAGER_HXX_
