/*************************************************************************
 *
 *  $RCSfile: statusbarmanager.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-17 14:58:32 $
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

#include <uielement/statusbarmanager.hxx>

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
#include <threadhelp/threadhelpbase.hxx>
#endif
#ifndef __FRAMEWORK_THREADHELP_RESETABLEGUARD_HXX_
#include <threadhelp/resetableguard.hxx>
#endif
#ifndef __FRAMEWORK_CLASSES_SFXHELPERFUNCTIONS_HXX_
#include <classes/sfxhelperfunctions.hxx>
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
#ifndef __FRAMEWORK_SERVICES_H_
#include "services.h"
#endif
#ifndef __FRAMEWORK_GENERAL_H_
#include "general.h"
#endif
#ifndef __FRAMEWORK_PROPERTIES_H_
#include "properties.h"
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
#ifndef _COM_SUN_STAR_UTIL_XUPDATABLE_HPP_
#include <com/sun/star/util/XUpdatable.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_UI_ITEMSTYLE_HPP_
#include <drafts/com/sun/star/ui/ItemStyle.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_UI_ITEMTYPE_HPP_
#include <drafts/com/sun/star/ui/ItemType.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTICOMPONENTFACTORY_HPP_
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_COMMAND_HPP_
#include <com/sun/star/awt/Command.hpp>
#endif

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/unohlp.hxx>
#endif
#ifndef _SVTOOLS_STATUSBARCONTROLLER_HXX
#include <svtools/statusbarcontroller.hxx>
#endif

#include <vcl/status.hxx>
#include <vcl/svapp.hxx>
#include <rtl/logfile.hxx>

using namespace ::com::sun::star;
#ifndef dcss
#define dcss ::drafts::com::sun::star
#endif
#ifndef dcss_ui
#define dcss_ui ::drafts::com::sun::star::ui
#endif

const sal_Int32  HELPID_PREFIX_LENGTH    = 7;
static const char*      HELPID_PREFIX           = "helpid:";

// Property names of a menu/menu item ItemDescriptor
static const char ITEM_DESCRIPTOR_COMMANDURL[]  = "CommandURL";
static const char ITEM_DESCRIPTOR_HELPURL[]     = "HelpURL";
static const char ITEM_DESCRIPTOR_OFFSET[]      = "Offset";
static const char ITEM_DESCRIPTOR_STYLE[]       = "Style";
static const char ITEM_DESCRIPTOR_WIDTH[]       = "Width";
static const char ITEM_DESCRIPTOR_TYPE[]        = "Type";

namespace framework
{

static USHORT impl_convertItemStyleToItemBits( sal_Int16 nStyle )
{
    USHORT nItemBits( 0 );

    if (( nStyle & dcss_ui::ItemStyle::ALIGN_RIGHT ) == dcss_ui::ItemStyle::ALIGN_RIGHT )
        nItemBits |= SIB_RIGHT;
    else if ( nStyle & dcss_ui::ItemStyle::ALIGN_LEFT )
        nItemBits |= SIB_LEFT;
    else
        nItemBits |= SIB_CENTER;

    if (( nStyle & dcss_ui::ItemStyle::DRAW_FLAT ) == dcss_ui::ItemStyle::DRAW_FLAT )
        nItemBits |= SIB_FLAT;
    else if ( nStyle & dcss_ui::ItemStyle::DRAW_OUT3D )
        nItemBits |= SIB_OUT;
    else
        nItemBits |= SIB_IN;

    if (( nStyle & dcss_ui::ItemStyle::AUTO_SIZE ) == dcss_ui::ItemStyle::AUTO_SIZE )
        nItemBits |= SIB_AUTOSIZE;
    if ( nStyle & dcss_ui::ItemStyle::OWNER_DRAW )
        nItemBits |= SIB_USERDRAW;

    return nItemBits;
}

//*****************************************************************************************************************
//  XInterface, XTypeProvider, XServiceInfo
//*****************************************************************************************************************
DEFINE_XINTERFACE_5                     (   StatusBarManager                                                        ,
                                            ::cppu::OWeakObject                                                     ,
                                            DIRECT_INTERFACE( lang::XTypeProvider                                   ),
                                            DIRECT_INTERFACE( lang::XComponent                                      ),
                                            DIRECT_INTERFACE( frame::XFrameActionListener                           ),
                                            DIRECT_INTERFACE( dcss::ui::XUIConfigurationListener                    ),
                                            DERIVED_INTERFACE( lang::XEventListener, frame::XFrameActionListener    )
                                        )

DEFINE_XTYPEPROVIDER_5                  (   StatusBarManager                    ,
                                            lang::XTypeProvider                 ,
                                            lang::XComponent                    ,
                                            dcss::ui::XUIConfigurationListener  ,
                                            frame::XFrameActionListener         ,
                                            lang::XEventListener
                                        )

StatusBarManager::StatusBarManager(
    const uno::Reference< lang::XMultiServiceFactory >& rServiceManager,
    const uno::Reference< frame::XFrame >& rFrame,
    const rtl::OUString& rResourceName,
    StatusBar* pStatusBar ) :
    ThreadHelpBase( &Application::GetSolarMutex() ),
    OWeakObject(),
    m_aListenerContainer( m_aLock.getShareableOslMutex() ),
    m_xServiceManager( rServiceManager ),
    m_xFrame( rFrame ),
    m_bDisposed( sal_False ),
    m_bModuleIdentified( sal_False ),
    m_bFrameActionRegistered( sal_False ),
    m_bUpdateControllers( sal_False ),
    m_aResourceName( rResourceName ),
    m_pStatusBar( pStatusBar )
{

    if ( m_xServiceManager.is() )
        m_xStatusbarControllerRegistration = uno::Reference< dcss::frame::XUIControllerRegistration >(
                                                    m_xServiceManager->createInstance( SERVICENAME_STATUSBARCONTROLLERFACTORY ),
                                                    uno::UNO_QUERY );

    m_pStatusBar->SetClickHdl( LINK( this, StatusBarManager, Click ) );
    m_pStatusBar->SetDoubleClickHdl( LINK( this, StatusBarManager, DoubleClick ) );
}

StatusBarManager::~StatusBarManager()
{
}

StatusBar* StatusBarManager::GetStatusBar() const
{
    ResetableGuard aGuard( m_aLock );
    return m_pStatusBar;
}

void StatusBarManager::frameAction( const frame::FrameActionEvent& Action )
throw ( uno::RuntimeException )
{
    ResetableGuard aGuard( m_aLock );
    if ( Action.Action == frame::FrameAction_CONTEXT_CHANGED )
        UpdateControllers();
}

void SAL_CALL StatusBarManager::disposing( const lang::EventObject& Source ) throw ( uno::RuntimeException )
{
    {
        ResetableGuard aGuard( m_aLock );
        if ( m_bDisposed )
            return;
    }

    RemoveControllers();

    {
        ResetableGuard aGuard( m_aLock );
        if ( Source.Source == uno::Reference< uno::XInterface >( m_xFrame, uno::UNO_QUERY ))
            m_xFrame.clear();

        m_xServiceManager.clear();
    }
}

// XComponent
void SAL_CALL StatusBarManager::dispose() throw( uno::RuntimeException )
{
    uno::Reference< lang::XComponent > xThis(
        static_cast< OWeakObject* >(this), uno::UNO_QUERY );

    lang::EventObject aEvent( xThis );
    m_aListenerContainer.disposeAndClear( aEvent );

    {
        ResetableGuard aGuard( m_aLock );
        if ( !m_bDisposed )
        {
            RemoveControllers();

            delete m_pStatusBar;
            m_pStatusBar = 0;

            if ( m_bFrameActionRegistered && m_xFrame.is() )
            {
                try
                {
                    m_xFrame->removeFrameActionListener( uno::Reference< frame::XFrameActionListener >(
                                                            static_cast< ::cppu::OWeakObject *>( this ),
                                                            uno::UNO_QUERY ));
                }
                catch ( uno::Exception& )
                {
                }
            }

            m_xFrame.clear();
            m_xServiceManager.clear();

            m_bDisposed = sal_True;
        }
    }
}

void SAL_CALL StatusBarManager::addEventListener( const uno::Reference< lang::XEventListener >& xListener ) throw( uno::RuntimeException )
{
    ResetableGuard aGuard( m_aLock );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    if ( m_bDisposed )
        throw lang::DisposedException();

    m_aListenerContainer.addInterface( ::getCppuType(
        ( const uno::Reference< lang::XEventListener >* ) NULL ), xListener );
}

void SAL_CALL StatusBarManager::removeEventListener( const uno::Reference< lang::XEventListener >& xListener ) throw( uno::RuntimeException )
{
    m_aListenerContainer.removeInterface( ::getCppuType(
        ( const uno::Reference< lang::XEventListener >* ) NULL ), xListener );
}

// XUIConfigurationListener
void SAL_CALL StatusBarManager::elementInserted( const dcss::ui::ConfigurationEvent& Event ) throw ( uno::RuntimeException )
{
    ResetableGuard aGuard( m_aLock );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    if ( m_bDisposed )
        return;
}

void SAL_CALL StatusBarManager::elementRemoved( const dcss::ui::ConfigurationEvent& Event ) throw ( uno::RuntimeException )
{
    ResetableGuard aGuard( m_aLock );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    if ( m_bDisposed )
        return;
}

void SAL_CALL StatusBarManager::elementReplaced( const dcss::ui::ConfigurationEvent& Event ) throw ( uno::RuntimeException )
{
    ResetableGuard aGuard( m_aLock );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    if ( m_bDisposed )
        return;
}

void StatusBarManager::UpdateControllers()
{
    if ( !m_bUpdateControllers )
    {
        m_bUpdateControllers = sal_True;
        for ( sal_uInt32 n = 0; n < m_aControllerVector.size(); n++ )
        {
            try
            {
                uno::Reference< util::XUpdatable > xUpdatable( m_aControllerVector[n], uno::UNO_QUERY );
                if ( xUpdatable.is() )
                    xUpdatable->update();
            }
            catch ( uno::Exception& )
            {
            }
        }
    }
    m_bUpdateControllers = sal_False;
}

void StatusBarManager::RemoveControllers()
{
    ResetableGuard aGuard( m_aLock );

    if ( m_bDisposed )
        return;

    for ( sal_uInt32 n = 0; n < m_aControllerVector.size(); n++ )
    {
        try
        {
            uno::Reference< lang::XComponent > xComponent(
                m_aControllerVector[n], uno::UNO_QUERY );
            if ( xComponent.is() )
                xComponent->dispose();
        }
        catch ( uno::Exception& )
        {
        }

        m_aControllerVector[n].clear();
    }
}

rtl::OUString StatusBarManager::RetrieveLabelFromCommand( const rtl::OUString& aCmdURL )
{
    rtl::OUString aLabel;

    // Retrieve short bubble help
    if ( !m_bModuleIdentified )
    {
        uno::Reference< dcss::frame::XModuleManager > xModuleManager(
            m_xServiceManager->createInstance(
                SERVICENAME_MODULEMANAGER ), uno::UNO_QUERY_THROW );
        uno::Reference< uno::XInterface > xIfac( m_xFrame, uno::UNO_QUERY );
        try
        {
            m_bModuleIdentified = sal_True;
            m_aModuleIdentifier = xModuleManager->identify( xIfac );

            if ( m_aModuleIdentifier.getLength() > 0 )
            {
                uno::Reference< container::XNameAccess > xNameAccess(
                    m_xServiceManager->createInstance(
                        SERVICENAME_UICOMMANDDESCRIPTION ), uno::UNO_QUERY );
                if ( xNameAccess.is() )
                {
                    uno::Any a = xNameAccess->getByName( m_aModuleIdentifier );
                    uno::Reference< container::XNameAccess > xUICommands;
                    a >>= m_xUICommandLabels;
                }
            }
        }
        catch ( uno::Exception& )
        {
        }
    }

    if ( m_xUICommandLabels.is() )
    {
        try
        {
            if ( aCmdURL.getLength() > 0 )
            {
                rtl::OUString aStr;
                uno::Sequence< beans::PropertyValue > aPropSeq;
                uno::Any a( m_xUICommandLabels->getByName( aCmdURL ));
                if ( a >>= aPropSeq )
                {
                    for ( sal_Int32 i = 0; i < aPropSeq.getLength(); i++ )
                    {
                        if ( aPropSeq[i].Name.equalsAscii( "Name" ))
                        {
                            aPropSeq[i].Value >>= aStr;
                            break;
                        }
                    }
                }
                aLabel = aStr;
            }
        }
        catch ( uno::Exception& )
        {
        }
    }

    return aLabel;
}

void StatusBarManager::CreateControllers()
{
    uno::Reference< lang::XMultiComponentFactory > xStatusbarControllerFactory( m_xStatusbarControllerRegistration, uno::UNO_QUERY );
    uno::Reference< uno::XComponentContext > xComponentContext;
    uno::Reference< beans::XPropertySet > xProps( m_xServiceManager, uno::UNO_QUERY );
    uno::Reference< awt::XWindow > xStatusbarWindow = VCLUnoHelper::GetInterface( m_pStatusBar );

    if ( xProps.is() )
        xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DefaultContext" ))) >>= xComponentContext;

    for ( USHORT i = 0; i < m_pStatusBar->GetItemCount(); i++ )
    {
        USHORT nId = m_pStatusBar->GetItemId( i );
        if ( nId == 0 )
            continue;

        rtl::OUString                            aCommandURL( m_pStatusBar->GetItemCommand( nId ));
        sal_Bool                                 bInit( sal_True );
        uno::Reference< frame::XStatusListener > xController;

        svt::StatusbarController* pController( 0 );

        if ( m_xStatusbarControllerRegistration.is() &&
             m_xStatusbarControllerRegistration->hasController( aCommandURL, m_aModuleIdentifier ))
        {
            if ( xStatusbarControllerFactory.is() )
            {
                uno::Sequence< uno::Any > aSeq( 5 );
                beans::PropertyValue aPropValue;

                aPropValue.Name     = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ModuleName" ));
                aPropValue.Value    = uno::makeAny( m_aModuleIdentifier );
                aSeq[0] = uno::makeAny( aPropValue );
                aPropValue.Name     = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Frame" ));
                aPropValue.Value    = uno::makeAny( m_xFrame );
                aSeq[1] = uno::makeAny( aPropValue );
                aPropValue.Name     = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ServiceManager" ));
                aPropValue.Value    = uno::makeAny( m_xServiceManager );
                aSeq[2] = uno::makeAny( aPropValue );
                aPropValue.Name     = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ParentWindow" ));
                aPropValue.Value    = uno::makeAny( xStatusbarWindow );
                aSeq[3] = uno::makeAny( aPropValue );
                aPropValue.Name     = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Identifier" ));
                aPropValue.Value    = uno::makeAny( nId );
                aSeq[4] = uno::makeAny( aPropValue );

                xController = uno::Reference< frame::XStatusListener >(
                                xStatusbarControllerFactory->createInstanceWithArgumentsAndContext(
                                    aCommandURL, aSeq, xComponentContext ),
                                uno::UNO_QUERY );
                bInit = sal_False; // Initialization is done through the factory service
            }
        }

        if ( !xController.is() )
        {
            pController = CreateStatusBarController( m_xFrame, m_pStatusBar, nId, aCommandURL );
            if ( !pController )
                pController = new svt::StatusbarController( m_xServiceManager, m_xFrame, aCommandURL, nId );

            if ( pController )
                xController = uno::Reference< frame::XStatusListener >(
                                static_cast< ::cppu::OWeakObject *>( pController ),
                                uno::UNO_QUERY );
        }

        m_aControllerVector.push_back( xController );
        uno::Reference< lang::XInitialization > xInit( xController, uno::UNO_QUERY );

        if ( xInit.is() )
        {
            if ( bInit )
            {
                beans::PropertyValue aPropValue;
                uno::Sequence< uno::Any > aArgs( 4 );
                aPropValue.Name     = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Frame" ));
                aPropValue.Value    = uno::makeAny( m_xFrame );
                aArgs[0] = uno::makeAny( aPropValue );
                aPropValue.Name     = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CommandURL" ));
                aPropValue.Value    = uno::makeAny( aCommandURL );
                aArgs[1] = uno::makeAny( aPropValue );
                aPropValue.Name     = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ServiceManager" ));
                aPropValue.Value    = uno::makeAny( m_xServiceManager );
                aArgs[2] = uno::makeAny( aPropValue );
                aPropValue.Name     = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ParentWindow" ));
                aPropValue.Value    = uno::makeAny( xStatusbarWindow );
                aArgs[3] = uno::makeAny( aPropValue );
                xInit->initialize( aArgs );
            }
        }
    }

    AddFrameActionListener();
}

void StatusBarManager::AddFrameActionListener()
{
    if ( !m_bFrameActionRegistered && m_xFrame.is() )
    {
        m_bFrameActionRegistered = sal_True;
        m_xFrame->addFrameActionListener( uno::Reference< frame::XFrameActionListener >(
            static_cast< ::cppu::OWeakObject *>( this ), uno::UNO_QUERY ));
    }
}

void StatusBarManager::FillStatusBar( const uno::Reference< container::XIndexAccess >& rItemContainer )
{
    RTL_LOGFILE_CONTEXT( aLog, "framework (cd100003) ::StatusBarManager::FillStatusbar" );

    ResetableGuard aGuard( m_aLock );

    if ( m_bDisposed || !m_pStatusBar )
        return;

    USHORT         nId( 1 );
    rtl::OUString  aHelpIdPrefix( RTL_CONSTASCII_USTRINGPARAM( HELPID_PREFIX ));

    RemoveControllers();

    // reset and fill command map
    m_pStatusBar->Clear();
    m_aControllerVector.clear();

    for ( sal_Int32 n = 0; n < rItemContainer->getCount(); n++ )
    {
        uno::Sequence< beans::PropertyValue >   aProp;
        rtl::OUString                           aCommandURL;
        rtl::OUString                           aHelpURL;
        sal_Int16                               nOffset( 0 );
        sal_Int16                               nStyle( 0 );
        sal_Int16                               nWidth( 0 );
        sal_uInt16                              nType( dcss_ui::ItemType::DEFAULT );

        try
        {
            if ( rItemContainer->getByIndex( n ) >>= aProp )
            {
                for ( int i = 0; i < aProp.getLength(); i++ )
                {
                    if ( aProp[i].Name.equalsAscii( ITEM_DESCRIPTOR_COMMANDURL ))
                    {
                        aProp[i].Value >>= aCommandURL;
                    }
                    else if ( aProp[i].Name.equalsAscii( ITEM_DESCRIPTOR_HELPURL ))
                    {
                        aProp[i].Value >>= aHelpURL;
                    }
                    else if ( aProp[i].Name.equalsAscii( ITEM_DESCRIPTOR_STYLE ))
                    {
                        aProp[i].Value >>= nStyle;
                    }
                    else if ( aProp[i].Name.equalsAscii( ITEM_DESCRIPTOR_TYPE ))
                    {
                        aProp[i].Value >>= nType;
                    }
                    else if ( aProp[i].Name.equalsAscii( ITEM_DESCRIPTOR_WIDTH ))
                    {
                        aProp[i].Value >>= nWidth;
                    }
                    else if ( aProp[i].Name.equalsAscii( ITEM_DESCRIPTOR_OFFSET ))
                    {
                        aProp[i].Value >>= nOffset;
                    }
                }

                if (( nType == drafts::com::sun::star::ui::ItemType::DEFAULT ) && ( aCommandURL.getLength() > 0 ))
                {
                    rtl::OUString aString( RetrieveLabelFromCommand( aCommandURL ));
                    USHORT        nItemBits( impl_convertItemStyleToItemBits( nStyle ));

                    m_pStatusBar->InsertItem( nId, nWidth, nItemBits, nOffset );
                    m_pStatusBar->SetItemCommand( nId, aCommandURL );
                    m_pStatusBar->SetAccessibleName( nId, aString );
//                    m_pStatusBar->SetHelpText( nId, aString );

                    if ( aHelpURL.indexOf( aHelpIdPrefix ) == 0 )
                    {
                        rtl::OUString aId( aHelpURL.copy( HELPID_PREFIX_LENGTH ));
                        sal_uInt16    nHelpId = (sal_uInt16)(aId.toInt32());
                        if ( nHelpId > 0 )
                            m_pStatusBar->SetHelpId( nId, nHelpId );
                    }

                    ++nId;
                }
            }
        }
        catch ( ::com::sun::star::lang::IndexOutOfBoundsException& )
        {
            break;
        }
    }

    // Create controllers
    CreateControllers();

    // Notify controllers that they are now correctly initialized and can start listening
    UpdateControllers();
}

void StatusBarManager::StateChanged( StateChangedType nType )
{
}

void StatusBarManager::DataChanged( const DataChangedEvent& rDCEvt )
{
}

void StatusBarManager::UserDraw( const UserDrawEvent& rUDEvt )
{
    ResetableGuard aGuard( m_aLock );

    if ( m_bDisposed )
        return;

    USHORT nId( rUDEvt.GetItemId() );
    if (( nId > 0 ) && ( nId <= m_aControllerVector.size() ))
    {
        uno::Reference< frame::XStatusbarController > xController(
            m_aControllerVector[nId-1], uno::UNO_QUERY );
        if ( xController.is() && rUDEvt.GetDevice() )
        {
            uno::Reference< awt::XGraphics > xGraphics =
                rUDEvt.GetDevice()->CreateUnoGraphics();

            awt::Rectangle aRect( rUDEvt.GetRect().Left(),
                                  rUDEvt.GetRect().Top(),
                                  rUDEvt.GetRect().GetWidth(),
                                  rUDEvt.GetRect().GetHeight() );
            aGuard.unlock();
            xController->paint( xGraphics, aRect, rUDEvt.GetItemId(), rUDEvt.GetStyle() );
        }
    }
}

void StatusBarManager::Command( const CommandEvent& rEvt )
{
    ResetableGuard aGuard( m_aLock );

    if ( m_bDisposed )
        return;

    if ( rEvt.GetCommand() == COMMAND_CONTEXTMENU )
    {
        USHORT nId = m_pStatusBar->GetItemId( rEvt.GetMousePosPixel() );
        if (( nId > 0 ) && ( nId <= m_aControllerVector.size() ))
        {
            uno::Reference< frame::XStatusbarController > xController(
                m_aControllerVector[nId-1], uno::UNO_QUERY );
            if ( xController.is() )
            {
                awt::Point aPos;
                aPos.X = rEvt.GetMousePosPixel().X();
                aPos.Y = rEvt.GetMousePosPixel().Y();
                xController->command( aPos, awt::Command::CONTEXTMENU, sal_True, uno::Any() );
            }
        }
    }
}

IMPL_LINK( StatusBarManager, Click, StatusBar*, pStatusBar )
{
    ResetableGuard aGuard( m_aLock );

    if ( m_bDisposed )
        return 1;

    USHORT nId = m_pStatusBar->GetCurItemId();
    if (( nId > 0 ) && ( nId <= m_aControllerVector.size() ))
    {
        uno::Reference< frame::XStatusbarController > xController(
            m_aControllerVector[nId-1], uno::UNO_QUERY );
        if ( xController.is() )
            xController->click();
    }

    return 1;
}

IMPL_LINK( StatusBarManager, DoubleClick, StatusBar*, pStatusBar )
{
    ResetableGuard aGuard( m_aLock );

    if ( m_bDisposed )
        return 1;

    USHORT nId = m_pStatusBar->GetCurItemId();
    if (( nId > 0 ) && ( nId <= m_aControllerVector.size() ))
    {
        uno::Reference< frame::XStatusbarController > xController(
            m_aControllerVector[nId-1], uno::UNO_QUERY );
        if ( xController.is() )
            xController->doubleClick();
    }

    return 1;
}

}
