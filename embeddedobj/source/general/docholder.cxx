/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: docholder.cxx,v $
 *
 *  $Revision: 1.28 $
 *
 *  last change: $Author: ihi $ $Date: 2006-12-19 14:03:57 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_embeddedobj.hxx"

#ifndef _COM_SUN_STAR_EMBED_ASPECTS_HPP_
#include <com/sun/star/embed/Aspects.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_XCOMPONENTCONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCOMPONENTLOADER_HPP_
#include <com/sun/star/frame/XComponentLoader.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSYNCHRONOUSFRAMELOADER_HPP_
#include <com/sun/star/frame/XSynchronousFrameLoader.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSINGLECOMPONENTFACTORY_HPP_
#include <com/sun/star/lang/XSingleComponentFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XCLOSEBROADCASTER_HPP_
#include <com/sun/star/util/XCloseBroadcaster.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XCLOSEABLE_HPP_
#include <com/sun/star/util/XCloseable.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDESKTOP_HPP_
#include <com/sun/star/frame/XDesktop.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAMESSUPPLIER_HPP_
#include <com/sun/star/frame/XFramesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHHELPER_HPP_
#include <com/sun/star/frame/XDispatchHelper.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_FRAMESEARCHFLAG_HPP_
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCONTROLLERBORDER_HPP_
#include <com/sun/star/frame/XControllerBorder.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XMODIFYBROADCASTER_HPP_
#include <com/sun/star/util/XModifyBroadcaster.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDERINTERCEPTION_HPP_
#include <com/sun/star/frame/XDispatchProviderInterception.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XTOPWINDOW_HPP_
#include <com/sun/star/awt/XTopWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_POSSIZE_HPP_
#include <com/sun/star/awt/PosSize.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XVIEW_HPP_
#include <com/sun/star/awt/XView.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_WINDOWATTRIBUTE_HPP_
#include <com/sun/star/awt/WindowAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_VCLWINDOWPEERATTRIBUTE_HPP_
#include <com/sun/star/awt/VclWindowPeerAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_BRIDGE_XBRIDGESUPPLIER2_HPP_
#include <com/sun/star/bridge/XBridgeSupplier2.hpp>
#endif
#ifndef _COM_SUN_STAR_BRIDGE_MODELDEPENDENT_HPP_
#include <com/sun/star/bridge/ModelDependent.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XHATCHWINDOW_HPP_
#include <com/sun/star/embed/XHatchWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XHATCHWINDOWFACTORY_HPP_
#include <com/sun/star/embed/XHatchWindowFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XINPLACECLIENT_HPP_
#include <com/sun/star/embed/XInplaceClient.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XLAYOUTMANAGER_HPP_
#include <com/sun/star/frame/XLayoutManager.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XMENUBARMERGINGACCEPTOR_HPP_
#include <com/sun/star/frame/XMenuBarMergingAcceptor.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XMODULEMANAGER_HPP_
#include <com/sun/star/frame/XModuleManager.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_XDOCKINGAREAACCEPTOR_HPP_
#include <com/sun/star/ui/XDockingAreaAcceptor.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_XUIELEMENTSETTINGS_HPP_
#include <com/sun/star/ui/XUIElementSettings.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_XCONFIGURATIONMANAGER_HPP_
#include <com/sun/star/ui/XUIConfigurationManager.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_XUICONFIGURATIONMANAGERSUPPLIER_HPP_
#include <com/sun/star/ui/XUIConfigurationManagerSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_XMODULEUICONFIGURATIONMANAGERSUPPLIER_HPP_
#include <com/sun/star/ui/XModuleUIConfigurationManagerSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_STATECHANGEINPROGRESSEXCEPTION_HPP_
#include <com/sun/star/embed/StateChangeInProgressException.hpp>
#endif

#include <com/sun/star/embed/EmbedMisc.hpp>
#include <com/sun/star/embed/EmbedStates.hpp>

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _RTL_PROCESS_H_
#include <rtl/process.h>
#endif

#include <comphelper/processfactory.hxx>

#include "docholder.hxx"
#include "commonembobj.hxx"
#include "intercept.hxx"


// #include <toolkit/helper/vclunohelper.hxx>
// #include <vcl/window.hxx>

#define HATCH_BORDER_WIDTH (((m_pEmbedObj->getStatus(embed::Aspects::MSOLE_CONTENT)&embed::EmbedMisc::MS_EMBED_ACTIVATEWHENVISIBLE) && \
                             m_pEmbedObj->getCurrentState()!=embed::EmbedStates::UI_ACTIVE) ? 0 : 4 )

using namespace ::com::sun::star;

//===========================================================================

class IntCounterGuard {
    sal_Int32& m_nFlag;
public:
    IntCounterGuard( sal_Int32& nFlag )
    : m_nFlag( nFlag )
    {
        m_nFlag++;
    }

    ~IntCounterGuard()
    {
        if ( m_nFlag )
            m_nFlag--;
    }
};

//===========================================================================

static void InsertMenu_Impl( const uno::Reference< container::XIndexContainer >& xTargetMenu,
                            sal_Int32 nTargetIndex,
                            const uno::Reference< container::XIndexAccess >& xSourceMenu,
                            sal_Int32 nSourceIndex,
                            const ::rtl::OUString aContModuleName )
{
    sal_Int32 nInd = 0;
    ::rtl::OUString aModuleIdentPropName( RTL_CONSTASCII_USTRINGPARAM( "ModuleIdentifier" ) );
    sal_Bool bModuleNameSet = sal_False;

    uno::Sequence< beans::PropertyValue > aSourceProps;
    xSourceMenu->getByIndex( nSourceIndex ) >>= aSourceProps;
    uno::Sequence< beans::PropertyValue > aTargetProps( aSourceProps.getLength() );
    for ( nInd = 0; nInd < aSourceProps.getLength(); nInd++ )
    {
        aTargetProps[nInd].Name = aSourceProps[nInd].Name;
        if ( aContModuleName.getLength() && aTargetProps[nInd].Name.equals( aModuleIdentPropName ) )
        {
            aTargetProps[nInd].Value <<= aContModuleName;
            bModuleNameSet = sal_True;
        }
        else
            aTargetProps[nInd].Value = aSourceProps[nInd].Value;
    }

    if ( !bModuleNameSet && aContModuleName.getLength() )
    {
        aTargetProps.realloc( nInd + 1 );
        aTargetProps[nInd].Name = aModuleIdentPropName;
        aTargetProps[nInd].Value <<= aContModuleName;
    }

    xTargetMenu->insertByIndex( nTargetIndex, uno::makeAny( aTargetProps ) );
}

//===========================================================================
DocumentHolder::DocumentHolder( const uno::Reference< lang::XMultiServiceFactory >& xFactory,
                                OCommonEmbeddedObject* pEmbObj )
: m_pEmbedObj( pEmbObj ),
  m_pInterceptor( NULL ),
  m_xFactory( xFactory ),
  m_bReadOnly( sal_False ),
  m_bWaitForClose( sal_False ),
  m_bAllowClosing( sal_False ),
  m_bDesktopTerminated( sal_False ),
  m_nNoBorderResizeReact( 0 ),
  m_nNoResizeReact( 0 )
{
    const ::rtl::OUString aServiceName ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.frame.Desktop" ) );
    uno::Reference< frame::XDesktop > xDesktop( m_xFactory->createInstance( aServiceName ), uno::UNO_QUERY );
    if ( xDesktop.is() )
    {
        m_refCount++;
        try
        {
            xDesktop->addTerminateListener( this );
        }
        catch ( uno::Exception& )
        {
        }
        m_refCount--;
    }
}

//---------------------------------------------------------------------------
DocumentHolder::~DocumentHolder()
{
    m_refCount++; // to allow deregistration as a listener

    if( m_xFrame.is() )
        CloseFrame();

    if ( m_xComponent.is() )
    {
        try {
            CloseDocument( sal_True, sal_False );
        } catch( uno::Exception& ) {}
    }

    if ( m_pInterceptor )
    {
        m_pInterceptor->DisconnectDocHolder();
        m_pInterceptor->release();
    }

    if ( !m_bDesktopTerminated )
        FreeOffice();
}

//---------------------------------------------------------------------------
void DocumentHolder::CloseFrame()
{
    uno::Reference< util::XCloseBroadcaster > xCloseBroadcaster( m_xFrame, uno::UNO_QUERY );
    if ( xCloseBroadcaster.is() )
        xCloseBroadcaster->removeCloseListener( ( util::XCloseListener* )this );

    uno::Reference<util::XCloseable> xCloseable(
        m_xFrame,uno::UNO_QUERY );
    if( xCloseable.is() )
        try {
            xCloseable->close( sal_True );
        }
        catch( const uno::Exception& ) {
        }
    else {
        uno::Reference<lang::XComponent> xComp( m_xFrame,uno::UNO_QUERY );
        if( xComp.is() )
            xComp->dispose();
    }

    uno::Reference< lang::XComponent > xComp( m_xHatchWindow, uno::UNO_QUERY );
    if ( xComp.is() )
        xComp->dispose();

    m_xHatchWindow = uno::Reference< awt::XWindow >();
    m_xOwnWindow = uno::Reference< awt::XWindow >();
    m_xFrame = uno::Reference< frame::XFrame >();
}

//---------------------------------------------------------------------------
void DocumentHolder::FreeOffice()
{
    const ::rtl::OUString aServiceName ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.frame.Desktop" ) );
    uno::Reference< frame::XDesktop > xDesktop( m_xFactory->createInstance( aServiceName ), uno::UNO_QUERY );
    if ( xDesktop.is() )
    {
        xDesktop->removeTerminateListener( this );

        // the following code is commented out since for now there is still no completely correct way to detect
        // whether the office can be terminated, so it is better to have unnecessary process running than
        // to loose any data

//      uno::Reference< frame::XFramesSupplier > xFramesSupplier( xDesktop, uno::UNO_QUERY );
//      if ( xFramesSupplier.is() )
//      {
//          uno::Reference< frame::XFrames > xFrames = xFramesSupplier->getFrames();
//          if ( xFrames.is() && !xFrames->hasElements() )
//          {
//              try
//              {
//                  xDesktop->terminate();
//              }
//              catch( uno::Exception & )
//              {}
//          }
//      }
    }
}

//---------------------------------------------------------------------------
void DocumentHolder::CloseDocument( sal_Bool bDeliverOwnership, sal_Bool bWaitForClose )
{
    uno::Reference< util::XCloseBroadcaster > xBroadcaster( m_xComponent, uno::UNO_QUERY );
    if ( xBroadcaster.is() )
    {
        uno::Reference< document::XEventBroadcaster > xEventBroadcaster( m_xComponent, uno::UNO_QUERY );
        if ( xEventBroadcaster.is() )
            xEventBroadcaster->removeEventListener( ( document::XEventListener* )this );

        uno::Reference< util::XCloseable > xCloseable( xBroadcaster, uno::UNO_QUERY );
        if ( xCloseable.is() )
        {
            m_bAllowClosing = sal_True;
            m_bWaitForClose = bWaitForClose;
            xCloseable->close( bDeliverOwnership );
        }
    }

    m_xComponent = 0;
}

//---------------------------------------------------------------------------
void DocumentHolder::PlaceFrame( const awt::Rectangle& aNewRect )
{
    OSL_ENSURE( m_xFrame.is() && m_xOwnWindow.is(),
                "The object does not have windows required for inplace mode!" );

    //TODO: may need mutex locking???
    if ( m_xFrame.is() && m_xOwnWindow.is() )
    {
        // the frame can be replaced only in inplace mode
        frame::BorderWidths aOldWidths;
        IntCounterGuard aGuard( m_nNoBorderResizeReact );

        do
        {
            aOldWidths = m_aBorderWidths;

            awt::Rectangle aHatchRect = AddBorderToArea( aNewRect );

            ResizeWindows_Impl( aHatchRect );

        } while ( aOldWidths.Left != m_aBorderWidths.Left
               || aOldWidths.Top != m_aBorderWidths.Top
               || aOldWidths.Right != m_aBorderWidths.Right
               || aOldWidths.Bottom != m_aBorderWidths.Bottom );

        m_aObjRect = aNewRect;
    }
}

//---------------------------------------------------------------------------
void DocumentHolder::ResizeWindows_Impl( const awt::Rectangle& aHatchRect )
{
    OSL_ENSURE( m_xFrame.is() && m_xOwnWindow.is() /*&& m_xHatchWindow.is()*/,
                "The object does not have windows required for inplace mode!" );
    if ( m_xHatchWindow.is() )
    {
        m_xOwnWindow->setPosSize( HATCH_BORDER_WIDTH,
                                  HATCH_BORDER_WIDTH,
                                  aHatchRect.Width - 2*HATCH_BORDER_WIDTH,
                                  aHatchRect.Height - 2*HATCH_BORDER_WIDTH,
                                  awt::PosSize::POSSIZE );

        // Window* pWindow = VCLUnoHelper::GetWindow( m_xOwnWindow );

        m_xHatchWindow->setPosSize( aHatchRect.X,
                                    aHatchRect.Y,
                                    aHatchRect.Width,
                                    aHatchRect.Height,
                                    awt::PosSize::POSSIZE );
    }
    else
        m_xOwnWindow->setPosSize( aHatchRect.X + HATCH_BORDER_WIDTH,
                                  aHatchRect.Y + HATCH_BORDER_WIDTH,
                                  aHatchRect.Width - 2*HATCH_BORDER_WIDTH,
                                  aHatchRect.Height - 2*HATCH_BORDER_WIDTH,
                                  awt::PosSize::POSSIZE );
}

//---------------------------------------------------------------------------
sal_Bool DocumentHolder::SetFrameLMVisibility( const uno::Reference< frame::XFrame >& xFrame, sal_Bool bVisible )
{
    sal_Bool bResult = sal_False;

    try
    {
        uno::Reference< ::com::sun::star::frame::XLayoutManager > xLayoutManager;
        uno::Reference< beans::XPropertySet > xPropSet( xFrame, uno::UNO_QUERY_THROW );
        xPropSet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "LayoutManager" ))) >>= xLayoutManager;
        if ( xLayoutManager.is() )
        {
            xLayoutManager->setVisible( bVisible );

            // MBA: locking is done only on the container LM, because it is not about hiding windows, it's about
            // giving up control over the component window (and stopping to listen for resize events of the container window)
            if ( bVisible )
                xLayoutManager->unlock();
            else
                xLayoutManager->lock();

            bResult = sal_True;
        }
    }
    catch( uno::Exception& )
    {}

    return bResult;
}

//---------------------------------------------------------------------------
sal_Bool DocumentHolder::ShowInplace( const uno::Reference< awt::XWindowPeer >& xParent,
                                      const awt::Rectangle& aRectangleToShow,
                                      const uno::Reference< frame::XDispatchProvider >& xContDisp )
{
    OSL_ENSURE( !m_xFrame.is(), "A frame exists already!" );

    if ( !m_xFrame.is() )
    {
        uno::Reference < frame::XModel > xModel( GetComponent(), uno::UNO_QUERY );
        awt::Rectangle aHatchRectangle = AddBorderToArea( aRectangleToShow );

        awt::Rectangle aOwnRectangle(  HATCH_BORDER_WIDTH,
                                    HATCH_BORDER_WIDTH,
                                    aHatchRectangle.Width - 2*HATCH_BORDER_WIDTH,
                                    aHatchRectangle.Height - 2*HATCH_BORDER_WIDTH );
        uno::Reference< awt::XWindow > xHWindow;
        uno::Reference< awt::XWindowPeer > xMyParent( xParent );

        if ( xModel.is() )
        {

            uno::Reference< embed::XHatchWindowFactory > xHatchFactory(
                    m_xFactory->createInstance(
                        ::rtl::OUString::createFromAscii( "com.sun.star.embed.HatchWindowFactory" ) ),
                    uno::UNO_QUERY );

            if ( !xHatchFactory.is() )
                throw uno::RuntimeException();

            uno::Reference< embed::XHatchWindow > xHatchWindow =
                            xHatchFactory->createHatchWindowInstance( xParent,
                                                                      aHatchRectangle,
                                                                      awt::Size( HATCH_BORDER_WIDTH, HATCH_BORDER_WIDTH ) );

            uno::Reference< awt::XWindowPeer > xHatchWinPeer( xHatchWindow, uno::UNO_QUERY );
            xHWindow = uno::Reference< awt::XWindow >( xHatchWinPeer, uno::UNO_QUERY );
            if ( !xHWindow.is() )
                throw uno::RuntimeException(); // TODO: can not create own window

            xHatchWindow->setController( uno::Reference< embed::XHatchWindowController >(
                                                static_cast< embed::XHatchWindowController* >( this ) ) );

            xMyParent = xHatchWinPeer;
        }
        else
        {
            aOwnRectangle.X += aHatchRectangle.X;
            aOwnRectangle.Y += aHatchRectangle.Y;
        }

        awt::WindowDescriptor aOwnWinDescriptor( awt::WindowClass_TOP,
                                                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("dockingwindow") ),
                                                xMyParent,
                                                0,
                                                awt::Rectangle(),//aOwnRectangle,
                                                awt::WindowAttribute::SHOW | awt::VclWindowPeerAttribute::CLIPCHILDREN );

        uno::Reference< awt::XToolkit > xToolkit(
                            m_xFactory->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.awt.Toolkit" ) ),
                            uno::UNO_QUERY );
        if ( !xToolkit.is() )
            throw uno::RuntimeException();

        uno::Reference< awt::XWindowPeer > xNewWinPeer = xToolkit->createWindow( aOwnWinDescriptor );
        uno::Reference< awt::XWindow > xOwnWindow( xNewWinPeer, uno::UNO_QUERY );
        if ( !xOwnWindow.is() )
            throw uno::RuntimeException(); // TODO: can not create own window

        // create a frame based on the specified window
        uno::Reference< frame::XFrame > xFrame(
                m_xFactory->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.frame.Frame" ) ),
                    uno::UNO_QUERY );
        if ( !xFrame.is() )
            throw uno::RuntimeException(); // TODO: can not create a new frame

        xFrame->initialize( xOwnWindow );

        // the dispatch provider may not be a frame
        uno::Reference< frame::XFramesSupplier > xFramesSupplier( xContDisp, uno::UNO_QUERY );
        if ( xFramesSupplier.is() )
        {
            uno::Reference< frame::XFrames > xFrames = xFramesSupplier->getFrames();
            if ( xFrames.is() )
                xFrames->append( xFrame );
        }

        m_xHatchWindow = xHWindow;
        m_xOwnWindow = xOwnWindow;
        m_xFrame = xFrame;

        if ( !SetFrameLMVisibility( m_xFrame, sal_False ) )
        {
            OSL_ENSURE( sal_False, "Can't deactivate LayoutManager!\n" );
            // TODO/LATER: error handling?
        }

        // m_bIsInplace = sal_True; TODO: ?

        uno::Reference< util::XCloseBroadcaster > xCloseBroadcaster( m_xFrame, uno::UNO_QUERY );
        if ( xCloseBroadcaster.is() )
            xCloseBroadcaster->addCloseListener( ( util::XCloseListener* )this );

        // TODO: some listeners to the frame and the window ( resize for example )
    }

    if ( m_xComponent.is() )
    {
        if ( !LoadDocToFrame( sal_True ) )
        {
            CloseFrame();
            return sal_False;
        }

        uno::Reference< frame::XControllerBorder > xControllerBorder( m_xFrame->getController(), uno::UNO_QUERY );
        if ( xControllerBorder.is() )
        {
            m_aBorderWidths = xControllerBorder->getBorder();
            xControllerBorder->addBorderResizeListener( (frame::XBorderResizeListener*)this );
        }

        PlaceFrame( aRectangleToShow );

        if ( m_xHatchWindow.is() )
            m_xHatchWindow->setVisible( sal_True );

        return sal_True;
    }

    return sal_False;
}

//---------------------------------------------------------------------------
uno::Reference< container::XIndexAccess > DocumentHolder::RetrieveOwnMenu_Impl()
{
    uno::Reference< container::XIndexAccess > xResult;

    uno::Reference< ::com::sun::star::ui::XUIConfigurationManagerSupplier > xUIConfSupplier(
                m_xComponent,
                uno::UNO_QUERY_THROW );
    uno::Reference< ::com::sun::star::ui::XUIConfigurationManager > xUIConfigManager(
                xUIConfSupplier->getUIConfigurationManager(),
                uno::UNO_QUERY_THROW );

    try
    {
        xResult = xUIConfigManager->getSettings(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "private:resource/menubar/menubar" ) ),
                sal_False );
    }
    catch( uno::Exception )
    {}

    if ( !xResult.is() )
    {
        // no internal document configuration, use the one from the module
        uno::Reference< ::com::sun::star::frame::XModuleManager > xModuleMan(
                m_xFactory->createInstance(
                    ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.ModuleManager" ) ) ),
                    uno::UNO_QUERY_THROW );
        ::rtl::OUString aModuleIdent =
            xModuleMan->identify( uno::Reference< uno::XInterface >( m_xComponent, uno::UNO_QUERY ) );

        if ( aModuleIdent.getLength() )
        {
            uno::Reference< ::com::sun::star::ui::XModuleUIConfigurationManagerSupplier > xModConfSupplier(
                    m_xFactory->createInstance( ::rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.ui.ModuleUIConfigurationManagerSupplier" ) ) ),
                    uno::UNO_QUERY_THROW );
            uno::Reference< ::com::sun::star::ui::XUIConfigurationManager > xModUIConfMan(
                    xModConfSupplier->getUIConfigurationManager( aModuleIdent ),
                    uno::UNO_QUERY_THROW );
            xResult = xModUIConfMan->getSettings(
                    ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "private:resource/menubar/menubar" ) ),
                    sal_False );
        }
    }

    if ( !xResult.is() )
        throw uno::RuntimeException();

    return xResult;
}

//---------------------------------------------------------------------------
void DocumentHolder::FindConnectPoints(
        const uno::Reference< container::XIndexAccess >& xMenu,
        sal_Int32 nConnectPoints[2] )
    throw ( uno::Exception )
{
    nConnectPoints[0] = -1;
    nConnectPoints[1] = -1;
    for ( sal_Int32 nInd = 0; nInd < xMenu->getCount(); nInd++ )
    {
        uno::Sequence< beans::PropertyValue > aProps;
        xMenu->getByIndex( nInd ) >>= aProps;
        rtl::OUString aCommand;
        for ( sal_Int32 nSeqInd = 0; nSeqInd < aProps.getLength(); nSeqInd++ )
            if ( aProps[nSeqInd].Name.equalsAscii( "CommandURL" ) )
            {
                aProps[nSeqInd].Value >>= aCommand;
                break;
            }

        if ( !aCommand.getLength() )
            throw uno::RuntimeException();

        if ( aCommand.equalsAscii( ".uno:PickList" ) )
            nConnectPoints[0] = nInd;
        else if ( aCommand.equalsAscii( ".uno:WindowList" ) )
            nConnectPoints[1] = nInd;
    }
}

//---------------------------------------------------------------------------
uno::Reference< container::XIndexAccess > DocumentHolder::MergeMenuesForInplace(
        const uno::Reference< container::XIndexAccess >& xContMenu,
        const uno::Reference< frame::XDispatchProvider >& /*xContDisp*/,
        const ::rtl::OUString& aContModuleName,
        const uno::Reference< container::XIndexAccess >& xOwnMenu,
        const uno::Reference< frame::XDispatchProvider >& /*xOwnDisp*/ )
    throw ( uno::Exception )
{
    // TODO/LATER: use dispatch providers on merge

    sal_Int32 nContPoints[2];
    sal_Int32 nOwnPoints[2];

    uno::Reference< lang::XSingleComponentFactory > xIndAccessFact( xContMenu, uno::UNO_QUERY_THROW );

    uno::Reference< uno::XComponentContext > xComponentContext;

    uno::Reference< beans::XPropertySet > xProps( ::comphelper::getProcessServiceFactory(), uno::UNO_QUERY );
    if ( xProps.is() )
        xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DefaultContext" ))) >>=
            xComponentContext;

    uno::Reference< container::XIndexContainer > xMergedMenu(
            xIndAccessFact->createInstanceWithContext( xComponentContext ),
            uno::UNO_QUERY_THROW );

    FindConnectPoints( xContMenu, nContPoints );
    FindConnectPoints( xOwnMenu, nOwnPoints );

    for ( sal_Int32 nInd = 0; nInd < xOwnMenu->getCount(); nInd++ )
    {
        if ( nOwnPoints[0] == nInd )
        {
            if ( nContPoints[0] >= 0 && nContPoints[0] < xContMenu->getCount() )
            {
                InsertMenu_Impl( xMergedMenu, nInd, xContMenu, nContPoints[0], aContModuleName );
            }
        }
        else if ( nOwnPoints[1] == nInd )
        {
            if ( nContPoints[1] >= 0 && nContPoints[1] < xContMenu->getCount() )
            {
                InsertMenu_Impl( xMergedMenu, nInd, xContMenu, nContPoints[1], aContModuleName );
            }
        }
        else
            InsertMenu_Impl( xMergedMenu, nInd, xOwnMenu, nInd, ::rtl::OUString() );
    }

    return uno::Reference< container::XIndexAccess >( xMergedMenu, uno::UNO_QUERY_THROW );
}

//---------------------------------------------------------------------------
sal_Bool DocumentHolder::MergeMenues_Impl( const uno::Reference< ::com::sun::star::frame::XLayoutManager >& xOwnLM,
                                               const uno::Reference< ::com::sun::star::frame::XLayoutManager >& xContLM,
                                            const uno::Reference< frame::XDispatchProvider >& xContDisp,
                                            const ::rtl::OUString& aContModuleName )
{
    sal_Bool bMenuMerged = sal_False;
    try
    {
        uno::Reference< ::com::sun::star::ui::XUIElementSettings > xUISettings(
            xContLM->getElement(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "private:resource/menubar/menubar" ) ) ),
            uno::UNO_QUERY_THROW );
        uno::Reference< container::XIndexAccess > xContMenu = xUISettings->getSettings( sal_True );
        if ( !xContMenu.is() )
            throw uno::RuntimeException();

        uno::Reference< container::XIndexAccess > xOwnMenu = RetrieveOwnMenu_Impl();
        uno::Reference< frame::XDispatchProvider > xOwnDisp( m_xFrame, uno::UNO_QUERY_THROW );

        uno::Reference< container::XIndexAccess > xMergedMenu = MergeMenuesForInplace( xContMenu, xContDisp, aContModuleName, xOwnMenu, xOwnDisp );
        uno::Reference< ::com::sun::star::frame::XMenuBarMergingAcceptor > xMerge( xOwnLM,
                                                                                         uno::UNO_QUERY_THROW );
        bMenuMerged = xMerge->setMergedMenuBar( xMergedMenu );
    }
    catch( uno::Exception& )
    {}

    return bMenuMerged;
}

sal_Bool DocumentHolder::ShowUI( const uno::Reference< ::com::sun::star::frame::XLayoutManager >& xContainerLM,
                                 const uno::Reference< frame::XDispatchProvider >& xContainerDP,
                                 const ::rtl::OUString& aContModuleName )
{
    sal_Bool bResult = sal_False;
    if ( xContainerLM.is() )
    {
        // the LM of the embedded frame and its current DockingAreaAcceptor
           uno::Reference< ::com::sun::star::frame::XLayoutManager > xOwnLM;
           uno::Reference< ::com::sun::star::ui::XDockingAreaAcceptor > xDocAreaAcc;

        try
        {
            uno::Reference< beans::XPropertySet > xPropSet( m_xFrame, uno::UNO_QUERY_THROW );
            xPropSet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "LayoutManager" ))) >>= xOwnLM;
            xDocAreaAcc = xContainerLM->getDockingAreaAcceptor();
        }
        catch( uno::Exception& ){}

        // make sure that lock state of LM is correct even if an exception is thrown in between
        sal_Bool bUnlock = sal_False;
        sal_Bool bLock = sal_False;
        if ( xOwnLM.is() && xDocAreaAcc.is() )
        {
            try
            {
                // take over the control over the containers window
                // as long as the LM is invisible and locked an empty tool space will be used on resizing
                xOwnLM->setDockingAreaAcceptor( xDocAreaAcc );

                // try to merge menues; don't do anything else if it fails
                if ( MergeMenues_Impl( xOwnLM, xContainerLM, xContainerDP, aContModuleName ) )
                {
                    // make sure that the container LM does not control the size of the containers window anymore
                    // this must be done after merging menues as we won't get the container menu otherwise
                    xContainerLM->setDockingAreaAcceptor( uno::Reference < ui::XDockingAreaAcceptor >() );

                    // prevent further changes at this LM
                    xContainerLM->setVisible( sal_False );
                       xContainerLM->lock();
                    bUnlock = sal_True;

                    // by unlocking the LM each layout change will now resize the containers window; pending layouts will be processed now
                    xOwnLM->setVisible( sal_True );

                    uno::Reference< frame::XFramesSupplier > xSupp( m_xFrame->getCreator(), uno::UNO_QUERY );
                    if ( xSupp.is() )
                        xSupp->setActiveFrame( m_xFrame );

                    xOwnLM->unlock();
                    bLock = sal_True;
                       bResult = sal_True;

                    // TODO/LATER: The following action should be done only if the window is not hidden
                    // otherwise the activation must fail, unfortunatelly currently it is not possible
                    // to detect whether the window is hidden using UNO API
                    m_xOwnWindow->setFocus();
                }
            }
            catch( uno::Exception& )
            {
                // activation failed; reestablish old state
                try
                {
                    uno::Reference< frame::XFramesSupplier > xSupp( m_xFrame->getCreator(), uno::UNO_QUERY );
                    if ( xSupp.is() )
                        xSupp->setActiveFrame( 0 );

                    // remove control about containers window from own LM
                    if ( bLock )
                        xOwnLM->lock();
                    xOwnLM->setVisible( sal_False );
                    xOwnLM->setDockingAreaAcceptor( uno::Reference< ::com::sun::star::ui::XDockingAreaAcceptor >() );

                    // unmerge menu
                    uno::Reference< ::com::sun::star::frame::XMenuBarMergingAcceptor > xMerge( xOwnLM, uno::UNO_QUERY_THROW );
                    xMerge->removeMergedMenuBar();
                }
                catch( uno::Exception& ) {}

                try
                {
                    // reestablish control of containers window
                    xContainerLM->setDockingAreaAcceptor( xDocAreaAcc );
                    xContainerLM->setVisible( sal_True );
                    if ( bUnlock )
                        xContainerLM->unlock();
                }
                catch( uno::Exception& ) {}
            }
        }
    }

    return bResult;
}

//---------------------------------------------------------------------------
sal_Bool DocumentHolder::HideUI( const uno::Reference< ::com::sun::star::frame::XLayoutManager >& xContainerLM )
{
    sal_Bool bResult = sal_False;

    if ( xContainerLM.is() )
    {
           uno::Reference< ::com::sun::star::frame::XLayoutManager > xOwnLM;

        try {
            uno::Reference< beans::XPropertySet > xPropSet( m_xFrame, uno::UNO_QUERY_THROW );
            xPropSet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "LayoutManager" ))) >>= xOwnLM;
        } catch( uno::Exception& )
        {}

        if ( xOwnLM.is() )
        {
            try {
                uno::Reference< frame::XFramesSupplier > xSupp( m_xFrame->getCreator(), uno::UNO_QUERY );
                if ( xSupp.is() )
                    xSupp->setActiveFrame( 0 );

                uno::Reference< ::com::sun::star::ui::XDockingAreaAcceptor > xDocAreaAcc = xOwnLM->getDockingAreaAcceptor();

                xOwnLM->setDockingAreaAcceptor( uno::Reference < ui::XDockingAreaAcceptor >() );
                xOwnLM->lock();
                xOwnLM->setVisible( sal_False );

                uno::Reference< ::com::sun::star::frame::XMenuBarMergingAcceptor > xMerge( xOwnLM, uno::UNO_QUERY_THROW );
                xMerge->removeMergedMenuBar();

                xContainerLM->setDockingAreaAcceptor( xDocAreaAcc );
                xContainerLM->setVisible( sal_True );
                xContainerLM->unlock();

                xContainerLM->doLayout();
                bResult = sal_True;
            }
            catch( uno::Exception& )
            {
                SetFrameLMVisibility( m_xFrame, sal_True );
            }
        }
    }

    return bResult;
}

//---------------------------------------------------------------------------
uno::Reference< frame::XFrame > DocumentHolder::GetDocFrame()
{
    // the frame for outplace activation
    if ( !m_xFrame.is() )
    {
        uno::Reference< frame::XFrame > xDesktopFrame(
            m_xFactory->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.frame.Desktop" ) ),
            uno::UNO_QUERY );

        if( !xDesktopFrame.is() )
            throw uno::RuntimeException();

        m_xFrame = xDesktopFrame->findFrame( rtl::OUString::createFromAscii( "_blank" ), 0 );

        uno::Reference< frame::XDispatchProviderInterception > xInterception( m_xFrame, uno::UNO_QUERY );
        if ( xInterception.is() )
        {
            if ( m_pInterceptor )
            {
                m_pInterceptor->DisconnectDocHolder();
                m_pInterceptor->release();
                m_pInterceptor = NULL;
            }

            m_pInterceptor = new Interceptor( this );
            m_pInterceptor->acquire();

            // register interceptor from outside
            if ( m_xOutplaceInterceptor.is() )
                xInterception->registerDispatchProviderInterceptor( m_xOutplaceInterceptor );

            xInterception->registerDispatchProviderInterceptor( m_pInterceptor );
        }

        uno::Reference< util::XCloseBroadcaster > xCloseBroadcaster( m_xFrame, uno::UNO_QUERY );
        if ( xCloseBroadcaster.is() )
            xCloseBroadcaster->addCloseListener( ( util::XCloseListener* )this );
    }

    if ( m_xComponent.is() )
    {
        uno::Reference< ::com::sun::star::frame::XLayoutManager > xOwnLM;
        try {
            uno::Reference< beans::XPropertySet > xPropSet( m_xFrame, uno::UNO_QUERY_THROW );
            xPropSet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "LayoutManager" ))) >>= xOwnLM;
        } catch( uno::Exception& )
        {}

        if ( xOwnLM.is() )
            xOwnLM->lock();

        // TODO/LATER: get it for the real aspect
        awt::Size aSize;
        GetExtent( embed::Aspects::MSOLE_CONTENT, &aSize );
        LoadDocToFrame(sal_False);

        if ( xOwnLM.is() )
        {
            xOwnLM->unlock();
            xOwnLM->lock();
        }

        SetExtent( embed::Aspects::MSOLE_CONTENT, aSize );

        if ( xOwnLM.is() )
            xOwnLM->unlock();
    }

    return m_xFrame;
}

//---------------------------------------------------------------------------
void DocumentHolder::SetComponent( const uno::Reference< util::XCloseable >& xDoc, sal_Bool bReadOnly )
{
    if ( m_xComponent.is() )
    {
        // May be should be improved
        try {
            CloseDocument( sal_True, sal_False );
        } catch( uno::Exception& )
        {}
    }

    m_xComponent = xDoc;
    // done outside currently uno::Reference < container::XChild > xChild( m_xComponent, uno::UNO_QUERY );
    // done outside currently if ( xChild.is() && m_pEmbedObj )
    // done outside currently   xChild->setParent( m_pEmbedObj->getParent() );

    m_bReadOnly = bReadOnly;
    m_bAllowClosing = sal_False;

    uno::Reference< util::XCloseBroadcaster > xBroadcaster( m_xComponent, uno::UNO_QUERY );
    if ( xBroadcaster.is() )
        xBroadcaster->addCloseListener( ( util::XCloseListener* )this );

    uno::Reference< document::XEventBroadcaster > xEventBroadcaster( m_xComponent, uno::UNO_QUERY );
    if ( xEventBroadcaster.is() )
        xEventBroadcaster->addEventListener( ( document::XEventListener* )this );

    if ( m_xFrame.is() )
        LoadDocToFrame(sal_False);
}

//---------------------------------------------------------------------------
sal_Bool DocumentHolder::LoadDocToFrame( sal_Bool bInPlace )
{
    if ( m_xFrame.is() && m_xComponent.is() )
    {
        uno::Reference < frame::XModel > xDoc( m_xComponent, uno::UNO_QUERY );
        if ( xDoc.is() )
        {
            // load new document in to the frame
            uno::Reference< frame::XComponentLoader > xComponentLoader( m_xFrame, uno::UNO_QUERY );
            if( !xComponentLoader.is() )
                throw uno::RuntimeException();

            uno::Sequence< beans::PropertyValue > aArgs( bInPlace ? 3 : 2 );
            aArgs[0].Name = ::rtl::OUString::createFromAscii( "Model" );
            aArgs[0].Value <<= m_xComponent;
            aArgs[1].Name = ::rtl::OUString::createFromAscii( "ReadOnly" );
            aArgs[1].Value <<= m_bReadOnly;
            if ( bInPlace )
            {
                aArgs[2].Name = ::rtl::OUString::createFromAscii( "PluginMode" );
                aArgs[2].Value <<= sal_Int16(1);
            }

            xComponentLoader->loadComponentFromURL( rtl::OUString::createFromAscii( "private:object" ),
                                                    rtl::OUString::createFromAscii( "_self" ),
                                                    0,
                                                    aArgs );

            ::rtl::OUString aDocumentName;
            uno::Reference < frame::XModel > xDocument( m_xComponent, uno::UNO_QUERY );

//REMOVE                uno::Sequence< beans::PropertyValue > aDocArgs = xDocument->getArgs();
//REMOVE                for ( sal_Int32 nInd = 0; nInd < aDocArgs.getLength(); nInd++ )
//REMOVE                    if ( aDocArgs[nInd].Name.equalsAscii( "Title" ) )
//REMOVE                    {
//REMOVE                        aDocArgs[nInd].Value >>= aDocumentName;
//REMOVE                        break;
//REMOVE                    }
//REMOVE
//REMOVE                SetTitle( aDocumentName );

            return sal_True;
        }
        else
        {
            uno::Reference < frame::XSynchronousFrameLoader > xLoader( m_xComponent, uno::UNO_QUERY );
            if ( xLoader.is() )
                return xLoader->load( uno::Sequence < beans::PropertyValue >(), m_xFrame );
            else
                return sal_False;
        }
    }

    return sal_True;
}

//---------------------------------------------------------------------------
void DocumentHolder::Show()
{
    if( m_xFrame.is() )
    {
        m_xFrame->activate();
        uno::Reference<awt::XTopWindow> xTopWindow( m_xFrame->getContainerWindow(), uno::UNO_QUERY );
        if( xTopWindow.is() )
            xTopWindow->toFront();
    }
    else
        GetDocFrame();
}

//---------------------------------------------------------------------------
void DocumentHolder::SetTitle( const rtl::OUString& aDocumentName )
{
    // TODO: to have a different title for links
    if( m_xFrame.is() )
    {
        rtl::OUString aFilterName;
        uno::Sequence<beans::PropertyValue> aSeq;
        uno::Reference < frame::XModel > xDocument( m_xComponent, uno::UNO_QUERY );
        if( xDocument.is() )
        {
            aSeq = xDocument->getArgs();
            for( sal_Int32 j = 0; j < aSeq.getLength(); ++j )
            {
                if( aSeq[j].Name == rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "FilterName" ) ) )
                {
                    aSeq[j].Value >>= aFilterName;
                    break;
                }
            }
        }

        if( aFilterName.getLength() )
        {
            uno::Reference<container::XNameAccess> xNameAccess(
                m_xFactory->createInstance(
                    rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.document.FilterFactory" ) ) ),
                uno::UNO_QUERY );
            try {
                if( xNameAccess.is() && ( xNameAccess->getByName( aFilterName ) >>= aSeq ) )
                {
                    for( sal_Int32 j = 0; j < aSeq.getLength(); ++j )
                        if( aSeq[j].Name == rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "UIName" ) ) )
                        {
                            aSeq[j].Value >>= aFilterName;
                            break;
                        }
                }
            }
            catch( const uno::Exception& )
            {
            }
        }

        // set the title
        uno::Reference<beans::XPropertySet> xPropSet( m_xFrame,uno::UNO_QUERY );
        if( xPropSet.is() )
        {
            uno::Any aAny;
            static const sal_Unicode u[] = { ' ', '(', ' ', 0 };
            static const sal_Unicode c[] = { ' ', ')', 0 };
            rtl::OUString aTotalName( aFilterName );
            aTotalName += rtl::OUString( u );
            aTotalName += aDocumentName;
            aTotalName += rtl::OUString( c );
            aAny <<= aTotalName;

            try
            {
                xPropSet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Title" ) ),
                                            aAny );
            }
            catch( const uno::Exception& )
            {
            }
        }
    }

    m_aDocumentNamePart = aDocumentName;

    if( m_pInterceptor )
        m_pInterceptor->GenerateFeatureStateEvent();
}

//---------------------------------------------------------------------------
void DocumentHolder::SetContainerName( const rtl::OUString& aContainerName )
{
    m_aContainerName = aContainerName;
}

//---------------------------------------------------------------------------
void DocumentHolder::Hide()
{
    if( m_xFrame.is() )
        m_xFrame->deactivate();
}

#if 0
//---------------------------------------------------------------------------
sal_Bool DocumentHolder::SetVisArea( const awt::Rectangle& aRect )
{
    uno::Reference < frame::XModel > xDocument( m_xComponent, uno::UNO_QUERY );
    if( xDocument.is() )
    if ( xDocument.is() )
    {
        uno::Sequence< beans::PropertyValue > aArgs = xDocument->getArgs();
        for ( sal_Int32 nInd = 0; nInd < aArgs.getLength(); nInd++ )
            if ( aArgs[nInd].Name.equalsAscii( "WinExtent" ) )
            {
                // should allways be there
                uno::Sequence< sal_Int32 > aSeqRect( 4 );

                aSeqRect[0] = aRect.X;
                aSeqRect[1] = aRect.Y;
                aSeqRect[2] = aRect.X + aRect.Width;
                aSeqRect[3] = aRect.Y + aRect.Height;

                aArgs[nInd].Value <<= aSeqRect;

                xDocument->attachResource( m_xComponent->getURL(), aArgs );
                return sal_True;
            }

        OSL_ENSURE( sal_False, "WinExtent seems not to be implemented!\n" );
    }

    return sal_False;
}

//---------------------------------------------------------------------------
sal_Bool DocumentHolder::GetVisArea( awt::Rectangle *pRect )
{
    uno::Reference < frame::XModel > xDocument( m_xComponent, uno::UNO_QUERY );
    if( xDocument.is() )
    if ( pRect && xDocument.is() )
    {
        uno::Sequence< beans::PropertyValue > aArgs = xDocument->getArgs();
        for ( sal_Int32 nInd = 0; nInd < aArgs.getLength(); nInd++ )
            if ( aArgs[nInd].Name.equalsAscii( "WinExtent" ) )
            {
                uno::Sequence< sal_Int32 > aRect;
                if ( ( aArgs[nInd].Value >>= aRect ) && aRect.getLength() == 4 )
                {
                    pRect->X   = aRect[0];
                    pRect->Y    = aRect[1];
                    pRect->Width  = aRect[2] - pRect->X;
                    pRect->Height = aRect[3] - pRect->Y;

                    return sal_True;
                }

                break;
            }
    }

    return sal_False;
}
#endif

//---------------------------------------------------------------------------
sal_Bool DocumentHolder::SetExtent( sal_Int64 nAspect, const awt::Size& aSize )
{
    uno::Reference< embed::XVisualObject > xDocVis( m_xComponent, uno::UNO_QUERY );
    if ( xDocVis.is() )
    {
        try
        {
            xDocVis->setVisualAreaSize( nAspect, aSize );
            return sal_True;
        }
        catch( uno::Exception& )
        {
            // TODO: Error handling
        }
    }

    return sal_False;
}

//---------------------------------------------------------------------------
sal_Bool DocumentHolder::GetExtent( sal_Int64 nAspect, awt::Size *pSize )
{
    uno::Reference< embed::XVisualObject > xDocVis( m_xComponent, uno::UNO_QUERY );
    if ( pSize && xDocVis.is() )
    {
        try
        {
            *pSize = xDocVis->getVisualAreaSize( nAspect );
            return sal_True;
        }
        catch( uno::Exception& )
        {
            // TODO: Error handling
        }
    }

    return sal_False;
}

//---------------------------------------------------------------------------
sal_Int32 DocumentHolder::GetMapUnit( sal_Int64 nAspect )
{
    uno::Reference< embed::XVisualObject > xDocVis( m_xComponent, uno::UNO_QUERY );
    if ( xDocVis.is() )
    {
        try
        {
            return xDocVis->getMapUnit( nAspect );
        }
        catch( uno::Exception& )
        {
            // TODO: Error handling
        }
    }

    return 0;
}

//---------------------------------------------------------------------------
awt::Rectangle DocumentHolder::CalculateBorderedArea( const awt::Rectangle& aRect )
{
    return awt::Rectangle( aRect.X + m_aBorderWidths.Left + HATCH_BORDER_WIDTH,
                             aRect.Y + m_aBorderWidths.Top + HATCH_BORDER_WIDTH,
                             aRect.Width - m_aBorderWidths.Left - m_aBorderWidths.Right - 2*HATCH_BORDER_WIDTH,
                             aRect.Height - m_aBorderWidths.Top - m_aBorderWidths.Bottom - 2*HATCH_BORDER_WIDTH );
}

//---------------------------------------------------------------------------
awt::Rectangle DocumentHolder::AddBorderToArea( const awt::Rectangle& aRect )
{
    return awt::Rectangle( aRect.X - m_aBorderWidths.Left - HATCH_BORDER_WIDTH,
                             aRect.Y - m_aBorderWidths.Top - HATCH_BORDER_WIDTH,
                             aRect.Width + m_aBorderWidths.Left + m_aBorderWidths.Right + 2*HATCH_BORDER_WIDTH,
                             aRect.Height + m_aBorderWidths.Top + m_aBorderWidths.Bottom + 2*HATCH_BORDER_WIDTH );
}

//---------------------------------------------------------------------------
void SAL_CALL DocumentHolder::disposing( const com::sun::star::lang::EventObject& aSource )
        throw (uno::RuntimeException)
{
    if ( m_xComponent.is() && m_xComponent == aSource.Source )
    {
        m_xComponent = 0;
        if ( m_bWaitForClose )
        {
            m_bWaitForClose = sal_False;
            FreeOffice();
        }
    }

    if( m_xFrame.is() && m_xFrame == aSource.Source )
    {
        m_xHatchWindow = uno::Reference< awt::XWindow >();
        m_xOwnWindow = uno::Reference< awt::XWindow >();
        m_xFrame = uno::Reference< frame::XFrame >();
    }
}


//---------------------------------------------------------------------------
void SAL_CALL DocumentHolder::queryClosing( const lang::EventObject& aSource, sal_Bool /*bGetsOwnership*/ )
        throw (util::CloseVetoException, uno::RuntimeException)
{
    if ( m_xComponent.is() && m_xComponent == aSource.Source && !m_bAllowClosing )
        throw util::CloseVetoException();
}

//---------------------------------------------------------------------------
void SAL_CALL DocumentHolder::notifyClosing( const lang::EventObject& aSource )
        throw (uno::RuntimeException)
{
    if ( m_xComponent.is() && m_xComponent == aSource.Source )
    {
        m_xComponent = 0;
        if ( m_bWaitForClose )
        {
            m_bWaitForClose = sal_False;
            FreeOffice();
        }
    }

    if( m_xFrame.is() && m_xFrame == aSource.Source )
    {
        m_xHatchWindow = uno::Reference< awt::XWindow >();
        m_xOwnWindow = uno::Reference< awt::XWindow >();
        m_xFrame = uno::Reference< frame::XFrame >();
    }
}

//---------------------------------------------------------------------------
void SAL_CALL DocumentHolder::queryTermination( const lang::EventObject& )
        throw (frame::TerminationVetoException, uno::RuntimeException)
{
    if ( m_bWaitForClose )
        throw frame::TerminationVetoException();
}

//---------------------------------------------------------------------------
void SAL_CALL DocumentHolder::notifyTermination( const lang::EventObject& aSource )
        throw (uno::RuntimeException)
{
    OSL_ENSURE( !m_xComponent.is(), "Just a disaster..." );

    uno::Reference< frame::XDesktop > xDesktop( aSource.Source, uno::UNO_QUERY );
    m_bDesktopTerminated = sal_True;
    if ( xDesktop.is() )
        xDesktop->removeTerminateListener( ( frame::XTerminateListener* )this );
}

//---------------------------------------------------------------------------
void SAL_CALL DocumentHolder::modified( const lang::EventObject& )
    throw ( uno::RuntimeException )
{
    if( m_pEmbedObj )
        m_pEmbedObj->update(); // TODO ???
}

//---------------------------------------------------------------------------
void SAL_CALL DocumentHolder::notifyEvent( const document::EventObject& Event )
    throw ( uno::RuntimeException )
{
    if( m_pEmbedObj && Event.Source == m_xComponent )
    {
        // for now the ignored events are not forwarded, but sent by the object itself
        if ( !Event.EventName.equalsAscii( "OnSave" )
          && !Event.EventName.equalsAscii( "OnSaveDone" )
          && !Event.EventName.equalsAscii( "OnSaveAs" )
          && !Event.EventName.equalsAscii( "OnSaveAsDone" )
          && !( Event.EventName.equalsAscii( "OnVisAreaChanged" ) && m_nNoResizeReact ) )
            m_pEmbedObj->PostEvent_Impl( Event.EventName, Event.Source );
    }
}

//---------------------------------------------------------------------------
void SAL_CALL DocumentHolder::borderWidthsChanged( const uno::Reference< uno::XInterface >& aObject,
                                                    const frame::BorderWidths& aNewSize )
    throw ( uno::RuntimeException )
{
    // TODO: may require mutex introduction ???
    if ( m_pEmbedObj && m_xFrame.is() && aObject == m_xFrame->getController() )
    {
        if ( m_aBorderWidths.Left != aNewSize.Left
          || m_aBorderWidths.Right != aNewSize.Right
          || m_aBorderWidths.Top != aNewSize.Top
          || m_aBorderWidths.Bottom != aNewSize.Bottom )
        {
            m_aBorderWidths = aNewSize;
            if ( !m_nNoBorderResizeReact )
                PlaceFrame( m_aObjRect );
        }
    }
}

//---------------------------------------------------------------------------
void SAL_CALL DocumentHolder::requestPositioning( const awt::Rectangle& aRect )
    throw (uno::RuntimeException)
{
    // TODO: may require mutex introduction ???
    if ( m_pEmbedObj )
    {
        // borders should not be counted
        awt::Rectangle aObjRect = CalculateBorderedArea( aRect );
        IntCounterGuard aGuard( m_nNoResizeReact );
        m_pEmbedObj->requestPositioning( aObjRect );
    }
}

//---------------------------------------------------------------------------
awt::Rectangle SAL_CALL DocumentHolder::calcAdjustedRectangle( const awt::Rectangle& aRect )
    throw (uno::RuntimeException)
{
    // Solar mutex should be locked already since this is a call from HatchWindow with focus
    awt::Rectangle aResult( aRect );

    if ( m_xFrame.is() )
    {
        // borders should not be counted
        uno::Reference< frame::XControllerBorder > xControllerBorder( m_xFrame->getController(), uno::UNO_QUERY );
        if ( xControllerBorder.is() )
        {
            awt::Rectangle aObjRect = CalculateBorderedArea( aRect );
            aObjRect = xControllerBorder->queryBorderedArea( aObjRect );
            aResult = AddBorderToArea( aObjRect );
        }
    }

    awt::Rectangle aMinRectangle = AddBorderToArea( awt::Rectangle() );
    if ( aResult.Width < aMinRectangle.Width + 2 )
        aResult.Width = aMinRectangle.Width + 2;
    if ( aResult.Height < aMinRectangle.Height + 2 )
        aResult.Height = aMinRectangle.Height + 2;

    return aResult;
}

void SAL_CALL DocumentHolder::activated(  ) throw (::com::sun::star::uno::RuntimeException)
{
    if ( (m_pEmbedObj->getStatus(embed::Aspects::MSOLE_CONTENT)&embed::EmbedMisc::MS_EMBED_ACTIVATEWHENVISIBLE) )
    {
        if ( m_pEmbedObj->getCurrentState() != embed::EmbedStates::UI_ACTIVE &&
        !(m_pEmbedObj->getStatus(embed::Aspects::MSOLE_CONTENT)&embed::EmbedMisc::MS_EMBED_NOUIACTIVATE) )
        {
            try
            {
                m_pEmbedObj->changeState( embed::EmbedStates::UI_ACTIVE );
            }
            catch ( com::sun::star::embed::StateChangeInProgressException& )
            {
                // must catch this exception because focus is grabbed while UI activation in doVerb()
            }
            catch ( com::sun::star::uno::Exception& )
            {
                // no outgoing exceptions specified here
            }
        }
        else
        {
            uno::Reference< frame::XFramesSupplier > xSupp( m_xFrame->getCreator(), uno::UNO_QUERY );
            if ( xSupp.is() )
                xSupp->setActiveFrame( m_xFrame );
        }
    }
}

void DocumentHolder::ResizeHatchWindow()
{
    awt::Rectangle aHatchRect = AddBorderToArea( m_aObjRect );
    ResizeWindows_Impl( aHatchRect );
    uno::Reference< embed::XHatchWindow > xHatchWindow( m_xHatchWindow, uno::UNO_QUERY );
    xHatchWindow->setHatchBorderSize( awt::Size( HATCH_BORDER_WIDTH, HATCH_BORDER_WIDTH ) );
}

void SAL_CALL DocumentHolder::deactivated(  ) throw (::com::sun::star::uno::RuntimeException)
{
    // deactivation is too unspecific to be useful; usually we only trigger code from activation
    // so UIDeactivation is actively triggered by the container
}
