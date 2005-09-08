/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: toolbarmanager.hxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:48:02 $
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

#ifndef __FRAMEWORK_UIELEMENT_TOOLBARMANAGER_HXX_
#define __FRAMEWORK_UIELEMENT_TOOLBARMANAGER_HXX_

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
#ifndef __FRAMEWORK_STDTYPES_H_
#include <stdtypes.h>
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTATUSLISTENER_HPP_
#include <com/sun/star/frame/XStatusListener.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXACCESS_HPP_
#include <com/sun/star/container/XIndexAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XMODULEMANAGER_HPP_
#include <com/sun/star/frame/XModuleManager.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XUICONTROLLERREGISTRATION_HPP_
#include <com/sun/star/frame/XUIControllerRegistration.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_XIMAGEMANAGER_HPP_
#include <com/sun/star/ui/XImageManager.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTATUSLISTENER_HPP_
#include <com/sun/star/frame/XStatusListener.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSUBTOOLBARCONTROLLER_HPP_
#include <com/sun/star/frame/XSubToolbarController.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_ITEMSTYLE_HPP_
#include <com/sun/star/ui/ItemStyle.hpp>
#endif

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef _RTL_USTRING_
#include <rtl/ustring.hxx>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

#ifndef _CPPUHELPER_INTERFACECONTAINER_HXX_
#include <cppuhelper/interfacecontainer.hxx>
#endif

#include <vcl/toolbox.hxx>


namespace framework
{

class ToolBar;
class ToolBarManager : public ::com::sun::star::frame::XFrameActionListener         ,
                       public ::com::sun::star::frame::XStatusListener              ,
                       public ::com::sun::star::lang::XComponent                    ,
                       public ::com::sun::star::lang::XTypeProvider                 ,
                       public ::com::sun::star::ui::XUIConfigurationListener,
                       public ThreadHelpBase                                        ,
                       public ::cppu::OWeakObject
{
    public:
        ToolBarManager( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rServicveManager,
                        const com::sun::star::uno::Reference< com::sun::star::frame::XFrame >& rFrame,
                        const rtl::OUString& rResourceName,
                        ToolBar* pToolBar );
        virtual ~ToolBarManager();

        //  XInterface, XTypeProvider, XServiceInfo
        DECLARE_XINTERFACE
        DECLARE_XTYPEPROVIDER

        ToolBox* GetToolBar() const;

        // XFrameActionListener
        virtual void SAL_CALL frameAction( const com::sun::star::frame::FrameActionEvent& Action ) throw ( ::com::sun::star::uno::RuntimeException );

        // XStatusListener
        virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event ) throw ( ::com::sun::star::uno::RuntimeException );

        // XEventListener
        virtual void SAL_CALL disposing( const com::sun::star::lang::EventObject& Source ) throw ( ::com::sun::star::uno::RuntimeException );

        // XUIConfigurationListener
        virtual void SAL_CALL elementInserted( const ::com::sun::star::ui::ConfigurationEvent& Event ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL elementRemoved( const ::com::sun::star::ui::ConfigurationEvent& Event ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL elementReplaced( const ::com::sun::star::ui::ConfigurationEvent& Event ) throw (::com::sun::star::uno::RuntimeException);

        // XComponent
        void SAL_CALL dispose() throw ( ::com::sun::star::uno::RuntimeException );
        void SAL_CALL addEventListener( const com::sun::star::uno::Reference< XEventListener >& xListener ) throw( com::sun::star::uno::RuntimeException );
        void SAL_CALL removeEventListener( const com::sun::star::uno::Reference< XEventListener >& xListener ) throw( com::sun::star::uno::RuntimeException );

        void CheckAndUpdateImages();
        void RefreshImages();
        void FillToolbar( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& rToolBarData );
        void notifyRegisteredControllers( const rtl::OUString& aUIElementName, const rtl::OUString& aCommand );
        void Destroy();
        static sal_Int16 GetCurrentSymbolSize();

    protected:
        struct ControllerParams
        {
            sal_Int16 nWidth;
        };
        typedef std::vector< ControllerParams > ControllerParamsVector;

        DECL_LINK( Click, ToolBox * );
        DECL_LINK( DropdownClick, ToolBox * );
        DECL_LINK( DoubleClick, ToolBox * );
        DECL_LINK( Select, ToolBox * );
        DECL_LINK( Highlight, ToolBox * );
        DECL_LINK( Activate, ToolBox * );
        DECL_LINK( Deactivate, ToolBox * );
        DECL_LINK( StateChanged, StateChangedType* );
        DECL_LINK( DataChanged, DataChangedEvent* );

        DECL_LINK( MenuButton, ToolBox * );
        DECL_LINK( MenuSelect, Menu * );
        DECL_LINK( MenuDeactivate, Menu * );
        DECL_LINK( AsyncUpdateControllersHdl, Timer * );

        void RemoveControllers();
        rtl::OUString RetrieveLabelFromCommand( const rtl::OUString& aCmdURL );
        void CreateControllers( const ControllerParamsVector& );
        void UpdateControllers();
        void AddFrameActionListener();
        void AddImageOrientationListener();
        void UpdateImageOrientation();
        void ImplClearPopupMenu( ToolBox *pToolBar );
        void RequestImages();
        sal_uInt16 ConvertStyleToToolboxItemBits( sal_Int32 nStyle );
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > GetModelFromFrame() const;
        sal_Bool IsPluginMode() const;

    protected:
        struct CommandInfo
        {
            CommandInfo() : nId( 0 ),
                            nImageInfo( 0 ),
                            bMirrored( false ),
                            bRotated( false ) {}
            USHORT                  nId;
            std::vector<USHORT>     aIds;
            sal_Int16               nImageInfo;
            sal_Bool                bMirrored : 1,
                                    bRotated  : 1;
        };

        typedef std::vector< ::com::sun::star::uno::Reference< com::sun::star::frame::XStatusListener > >           ToolBarControllerVector;
        typedef ::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::frame::XSubToolbarController > > SubToolBarControllerVector;
        typedef BaseHash< CommandInfo >                                                                             CommandToInfoMap;
        typedef BaseHash< SubToolBarControllerVector >                                                              SubToolBarToSubToolBarControllerMap;

        sal_Bool                                                                                        m_bDisposed : 1,
                                                                                                        m_bIsHiContrast : 1,
                                                                                                        m_bSmallSymbols : 1,
                                                                                                        m_bModuleIdentified : 1,
                                                                                                        m_bAddedToTaskPaneList : 1,
                                                                                                        m_bVerticalTextEnabled : 1,
                                                                                                        m_bFrameActionRegistered : 1,
                                                                                                        m_bUpdateControllers : 1;
        sal_Bool                                                                                        m_bImageOrientationRegistered : 1,
                                                                                                        m_bImageMirrored : 1,
                                                                                                        m_bCanBeCustomized : 1;
        long                                                                                            m_lImageRotation;
        ToolBar*                                                                                        m_pToolBar;
        rtl::OUString                                                                                   m_aModuleIdentifier;
        rtl::OUString                                                                                   m_aResourceName;
        com::sun::star::uno::Reference< com::sun::star::frame::XFrame >                                 m_xFrame;
        com::sun::star::uno::Reference< com::sun::star::container::XNameAccess >                        m_xUICommandLabels;
        ToolBarControllerVector                                                                         m_aControllerVector;
        ::cppu::OMultiTypeInterfaceContainerHelper                                                      m_aListenerContainer;   /// container for ALL Listener
        ::com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >                  m_xServiceManager;
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XUIControllerRegistration >  m_xToolbarControllerRegistration;
        ::com::sun::star::uno::Reference< ::com::sun::star::ui::XImageManager >                 m_xModuleImageManager;
        ::com::sun::star::uno::Reference< ::com::sun::star::ui::XImageManager >                 m_xDocImageManager;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >                          m_xImageOrientationListener;
        CommandToInfoMap                                                                                m_aCommandMap;
        SubToolBarToSubToolBarControllerMap                                                             m_aSubToolBarControllerMap;
        Timer                                                                                           m_aAsyncUpdateControllersTimer;
};

}

#endif // __FRAMEWORK_UIELEMENT_TOOLBARMANAGER_HXX_
