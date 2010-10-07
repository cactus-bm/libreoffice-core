/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: layoutmanager.cxx,v $
 * $Revision: 1.72 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_framework.hxx"

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#include <toolbarlayoutmanager.hxx>
#include <helpers.hxx>
#include <services.h>
#include <classes/resource.hrc>
#include <classes/fwkresid.hxx>
#include <uiconfiguration/windowstateconfiguration.hxx>

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#include <com/sun/star/awt/PosSize.hpp>
#include <com/sun/star/ui/UIElementType.hpp>
#include <com/sun/star/container/XNameReplace.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/ui/XUIElementSettings.hpp>
#include <com/sun/star/ui/XUIFunctionListener.hpp>

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#include <unotools/cmdoptions.hxx>
#include <toolkit/unohlp.hxx>
#include <toolkit/helper/convert.hxx>
#include <toolkit/awt/vclxwindow.hxx>
#include <vcl/i18nhelp.hxx>
#include <boost/bind.hpp>

using namespace ::com::sun::star;

namespace framework
{

ToolbarLayoutManager::ToolbarLayoutManager(
    const uno::Reference< lang::XMultiServiceFactory >& xSMGR,
    const uno::Reference< ui::XUIElementFactory >& xUIElementFactory,
    ILayoutNotifications* pParentLayouter )
    : ThreadHelpBase( &Application::GetSolarMutex() ),
    m_xSMGR( xSMGR ),
    m_xUIElementFactoryManager( xUIElementFactory ),
    m_pParentLayouter( pParentLayouter ),
    m_eDockOperation( DOCKOP_ON_COLROW ),
    m_pAddonOptions( 0 ),
    m_pGlobalSettings( 0 ),
    m_bComponentAttached( false ),
    m_bMustLayout( false ),
    m_bLayoutDirty( false ),
    m_bStoreWindowState( false ),
    m_bGlobalSettings( false ),
    m_bDockingInProgress( false ),
    m_bVisible( true ),
    m_bLayoutInProgress( false ),
    m_aFullAddonTbxPrefix( RTL_CONSTASCII_USTRINGPARAM( "private:resource/toolbar/addon_" )),
    m_aCustomTbxPrefix( RTL_CONSTASCII_USTRINGPARAM( "custom_" )),
    m_aCustomizeCmd( RTL_CONSTASCII_USTRINGPARAM( "ConfigureDialog" )),
    m_aToolbarTypeString( RTL_CONSTASCII_USTRINGPARAM( UIRESOURCETYPE_TOOLBAR ))
{
    // initialize rectangles to zero values
    setZeroRectangle( m_aDockingAreaOffsets );
    setZeroRectangle( m_aDockingArea );

    // create toolkit object
    m_xToolkit = uno::Reference< css::awt::XToolkit >( m_xSMGR->createInstance( SERVICENAME_VCLTOOLKIT ), uno::UNO_QUERY );
}

ToolbarLayoutManager::~ToolbarLayoutManager()
{
}

//---------------------------------------------------------------------------------------------------------
//  XInterface
//---------------------------------------------------------------------------------------------------------
void SAL_CALL ToolbarLayoutManager::acquire() throw()
{
    OWeakObject::acquire();
}

void SAL_CALL ToolbarLayoutManager::release() throw()
{
    OWeakObject::release();
}

uno::Any SAL_CALL ToolbarLayoutManager::queryInterface( const uno::Type & rType ) throw( uno::RuntimeException )
{
    uno::Any a =
        ::cppu::queryInterface( rType,
            SAL_STATIC_CAST( ::com::sun::star::awt::XDockableWindowListener*, this ),
            SAL_STATIC_CAST( ::com::sun::star::ui::XUIConfigurationListener*, this ),
            SAL_STATIC_CAST( ::com::sun::star::awt::XWindowListener*,         this ));

    if ( a.hasValue() )
        return a;

    return OWeakObject::queryInterface( rType );
}

void SAL_CALL ToolbarLayoutManager::disposing( const lang::EventObject& aEvent ) throw( uno::RuntimeException )
{
    if ( aEvent.Source == m_xFrame )
    {
        // Reset all internal references
        reset();
        implts_destroyDockingAreaWindows();
    }
}

::com::sun::star::awt::Rectangle ToolbarLayoutManager::getDockingArea()
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    Rectangle aNewDockingArea( m_aDockingArea );
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    if ( isLayoutDirty() )
        aNewDockingArea = implts_calcDockingArea();

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    aWriteLock.lock();
    m_aDockingArea = aNewDockingArea;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    return putRectangleValueToAWT(aNewDockingArea);
}

void ToolbarLayoutManager::setDockingArea( const awt::Rectangle& rDockingArea )
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    m_aDockingArea = putAWTToRectangle( rDockingArea );
    m_bLayoutDirty = true;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    implts_setDockingAreaWindowSizes( rDockingArea );
}

void ToolbarLayoutManager::implts_setDockingAreaWindowSizes( const awt::Rectangle& rBorderSpace )
{
    uno::Reference< css::awt::XWindow2 > xContainerWindow;

    ReadGuard aReadLock( m_aLock );
    xContainerWindow = m_xContainerWindow;
    Rectangle aDockOffsets = m_aDockingAreaOffsets;
    aReadLock.unlock();

    uno::Reference< awt::XDevice > xDevice( xContainerWindow, uno::UNO_QUERY );

    // Convert relativ size to output size.
    awt::Rectangle  aRectangle           = xContainerWindow->getPosSize();
    awt::DeviceInfo aInfo                = xDevice->getInfo();
    awt::Size       aContainerClientSize = awt::Size( aRectangle.Width - aInfo.LeftInset - aInfo.RightInset  ,
                                                      aRectangle.Height - aInfo.TopInset  - aInfo.BottomInset );
    long aStatusBarHeight                = aDockOffsets.GetHeight();

    sal_Int32 nLeftRightDockingAreaHeight( aContainerClientSize.Height );
    if ( rBorderSpace.Y >= 0 )
    {
        // Top docking area window
        aReadLock.lock();
        uno::Reference< awt::XWindow > xDockAreaWindow( m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_TOP] );
        aReadLock.unlock();

        xDockAreaWindow->setPosSize( 0, 0, aContainerClientSize.Width, rBorderSpace.Y, awt::PosSize::POSSIZE );
        xDockAreaWindow->setVisible( sal_True );
        nLeftRightDockingAreaHeight -= rBorderSpace.Y;
    }

    if ( rBorderSpace.Height >= 0 )
    {
        // Bottom docking area window
        sal_Int32 nBottomPos = std::max( sal_Int32( aContainerClientSize.Height - rBorderSpace.Height - aStatusBarHeight ), sal_Int32( 0 ));
        sal_Int32 nHeight = ( nBottomPos == 0 ) ? 0 : rBorderSpace.Height;

        aReadLock.lock();
        uno::Reference< awt::XWindow > xDockAreaWindow( m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_BOTTOM] );
        aReadLock.unlock();

        xDockAreaWindow->setPosSize( 0, nBottomPos, aContainerClientSize.Width, nHeight, awt::PosSize::POSSIZE );
        xDockAreaWindow->setVisible( sal_True );
        nLeftRightDockingAreaHeight -= nHeight;
    }

    nLeftRightDockingAreaHeight -= aStatusBarHeight;
    if ( rBorderSpace.X >= 0 || nLeftRightDockingAreaHeight > 0 )
    {
        // Left docking area window
        aReadLock.lock();
        uno::Reference< awt::XWindow > xDockAreaWindow( m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_LEFT] );
        aReadLock.unlock();

        // We also have to change our right docking area window if the top or bottom area has changed. They have a higher priority!
        sal_Int32 nHeight = std::max( sal_Int32( 0 ), sal_Int32( nLeftRightDockingAreaHeight ));

        xDockAreaWindow->setPosSize( 0, rBorderSpace.Y, rBorderSpace.X, nHeight, awt::PosSize::POSSIZE );
        xDockAreaWindow->setVisible( sal_True );
    }
    if ( rBorderSpace.Width >= 0 || nLeftRightDockingAreaHeight > 0 )
    {
        // Right docking area window
        aReadLock.lock();
        uno::Reference< awt::XWindow > xDockAreaWindow( m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_RIGHT] );
        aReadLock.unlock();

        // We also have to change our right docking area window if the top or bottom area has changed. They have a higher priority!
        sal_Int32 nLeftPos  = std::max( sal_Int32( 0 ), sal_Int32( aContainerClientSize.Width - rBorderSpace.Width ));
        sal_Int32 nHeight   = std::max( sal_Int32( 0 ), sal_Int32( nLeftRightDockingAreaHeight ));
        sal_Int32 nWidth    = ( nLeftPos == 0 ) ? 0 : rBorderSpace.Width;

        xDockAreaWindow->setPosSize( nLeftPos, rBorderSpace.Y, nWidth, nHeight, awt::PosSize::POSSIZE );
        xDockAreaWindow->setVisible( sal_True );
    }
}

bool ToolbarLayoutManager::isLayoutDirty()
{
    return m_bLayoutDirty;
}

void ToolbarLayoutManager::doLayout(const ::Size& aContainerSize)
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );

    if ( m_bLayoutInProgress )
        return;

    m_bLayoutInProgress = true;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    // Retrieve row/column dependent data from all docked user-interface elements
    for ( sal_Int32 i = 0; i < DOCKINGAREAS_COUNT; i++ )
    {
        std::vector< SingleRowColumnWindowData > aRowColumnsWindowData;

        implts_getDockingAreaElementInfos( (ui::DockingArea)i, aRowColumnsWindowData );

        sal_Int32 nOffset( 0 );
        const sal_uInt32 nCount = aRowColumnsWindowData.size();
        for ( sal_uInt32 j = 0; j < nCount; ++j )
        {
            implts_calcWindowPosSizeOnSingleRowColumn( i, nOffset, aRowColumnsWindowData[j], aContainerSize );
            nOffset += aRowColumnsWindowData[j].nStaticSize;
        }
    }

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    aWriteLock.lock();
    m_bLayoutDirty      = false;
    m_bLayoutInProgress = false;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
}

bool ToolbarLayoutManager::implts_isParentWindowVisible() const
{
    bool bVisible( false );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    if ( m_xContainerWindow.is() )
        bVisible = m_xContainerWindow->isVisible();
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    return bVisible;
}

Rectangle ToolbarLayoutManager::implts_calcDockingArea()
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    UIElementVector aWindowVector( m_aUIElements );
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    Rectangle                aBorderSpace;
    sal_Int32                nCurrRowColumn( 0 );
    sal_Int32                nCurrPos( 0 );
    sal_Int32                nCurrDockingArea( ui::DockingArea_DOCKINGAREA_TOP );
    std::vector< sal_Int32 > aRowColumnSizes[DOCKINGAREAS_COUNT];
    UIElementVector::const_iterator pConstIter;

    // initialize rectangle with zero values!
    aBorderSpace.setWidth(0);
    aBorderSpace.setHeight(0);

    aRowColumnSizes[nCurrDockingArea].clear();
    aRowColumnSizes[nCurrDockingArea].push_back( 0 );

    for ( pConstIter = aWindowVector.begin(); pConstIter != aWindowVector.end(); pConstIter++ )
    {
        uno::Reference< ui::XUIElement > xUIElement( pConstIter->m_xUIElement, uno::UNO_QUERY );
        if ( xUIElement.is() )
        {
            uno::Reference< awt::XWindow > xWindow( xUIElement->getRealInterface(), uno::UNO_QUERY );
            uno::Reference< awt::XDockableWindow > xDockWindow( xWindow, uno::UNO_QUERY );
            if ( xWindow.is() && xDockWindow.is() )
            {
                vos::OGuard aGuard( Application::GetSolarMutex() );

                Window* pWindow = VCLUnoHelper::GetWindow( xWindow );
                if ( pWindow && pWindow->IsVisible() && !xDockWindow->isFloating() )
                {
                    awt::Rectangle aPosSize = xWindow->getPosSize();
                    if ( pConstIter->m_aDockedData.m_nDockedArea != nCurrDockingArea )
                    {
                        nCurrDockingArea    = pConstIter->m_aDockedData.m_nDockedArea;
                        nCurrRowColumn      = 0;
                        nCurrPos            = 0;
                        aRowColumnSizes[nCurrDockingArea].clear();
                        aRowColumnSizes[nCurrDockingArea].push_back( 0 );
                    }

                    if ( pConstIter->m_aDockedData.m_nDockedArea == nCurrDockingArea )
                    {
                        if (( pConstIter->m_aDockedData.m_nDockedArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
                            ( pConstIter->m_aDockedData.m_nDockedArea == ui::DockingArea_DOCKINGAREA_BOTTOM ))
                        {
                            if ( pConstIter->m_aDockedData.m_aPos.Y() > nCurrPos )
                            {
                                ++nCurrRowColumn;
                                nCurrPos = pConstIter->m_aDockedData.m_aPos.Y();
                                aRowColumnSizes[nCurrDockingArea].push_back( 0 );
                            }

                            if ( aPosSize.Height > aRowColumnSizes[nCurrDockingArea][nCurrRowColumn] )
                                aRowColumnSizes[nCurrDockingArea][nCurrRowColumn] = aPosSize.Height;
                        }
                        else
                        {
                            if ( pConstIter->m_aDockedData.m_aPos.X() > nCurrPos )
                            {
                                ++nCurrRowColumn;
                                nCurrPos = pConstIter->m_aDockedData.m_aPos.X();
                                aRowColumnSizes[nCurrDockingArea].push_back( 0 );
                            }

                            if ( aPosSize.Width > aRowColumnSizes[nCurrDockingArea][nCurrRowColumn] )
                                aRowColumnSizes[nCurrDockingArea][nCurrRowColumn] = aPosSize.Width;
                        }
                    }
                }
            }
        }
    }

    // Sum up max heights from every row/column
    if ( !aWindowVector.empty() )
    {
        for ( sal_Int32 i = 0; i <= ui::DockingArea_DOCKINGAREA_RIGHT; i++ )
        {
            sal_Int32 nSize( 0 );
            const sal_uInt32 nCount = aRowColumnSizes[i].size();
            for ( sal_uInt32 j = 0; j < nCount; j++ )
                nSize += aRowColumnSizes[i][j];

            if ( i == ui::DockingArea_DOCKINGAREA_TOP )
                aBorderSpace.Top() = nSize;
            else if ( i == ui::DockingArea_DOCKINGAREA_BOTTOM )
                aBorderSpace.Bottom() = nSize;
            else if ( i == ui::DockingArea_DOCKINGAREA_LEFT )
                aBorderSpace.Left() = nSize;
            else
                aBorderSpace.Right() = nSize;
        }
    }

    return aBorderSpace;
}

void ToolbarLayoutManager::reset()
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    uno::Reference< ui::XUIConfigurationManager > xModuleCfgMgr = m_xModuleCfgMgr;
    uno::Reference< ui::XUIConfigurationManager > xDocCfgMgr    = m_xDocCfgMgr;
    m_xModuleCfgMgr.clear();
    m_xDocCfgMgr.clear();
    m_bComponentAttached = false;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    destroyToolbars();
    resetDockingArea();
}

void ToolbarLayoutManager::attach(
    const uno::Reference< frame::XFrame >& xFrame,
    const uno::Reference< ui::XUIConfigurationManager >& xModuleCfgMgr,
    const uno::Reference< ui::XUIConfigurationManager >& xDocCfgMgr,
    const uno::Reference< container::XNameAccess >& xPersistentWindowState )
{
    // reset toolbar manager if we lose our current frame
    if ( m_xFrame.is() && m_xFrame != xFrame )
        reset();

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    m_xFrame                 = xFrame;
    m_xModuleCfgMgr          = xModuleCfgMgr;
    m_xDocCfgMgr             = xDocCfgMgr;
    m_xPersistentWindowState = xPersistentWindowState;
    m_bComponentAttached     = true;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
}

void ToolbarLayoutManager::createStaticToolbars()
{
    resetDockingArea();
    implts_createCustomToolBars();
    implts_createAddonsToolBars();
    implts_createNonContextSensitiveToolBars();
    implts_sortUIElements();
}

bool ToolbarLayoutManager::requestToolbar( const ::rtl::OUString& rResourceURL )
{
    bool                             bNotify( false );
    bool                             bMustCallCreate( false );
    uno::Reference< ui::XUIElement > xUIElement;

    UIElement aRequestedToolbar = impl_findToolbar( rResourceURL );
    if ( aRequestedToolbar.m_aName != rResourceURL  )
    {
        bMustCallCreate = true;
        aRequestedToolbar.m_aName      = rResourceURL;
        aRequestedToolbar.m_aType      = m_aToolbarTypeString;
        aRequestedToolbar.m_xUIElement = xUIElement;
        implts_readWindowStateData( rResourceURL, aRequestedToolbar );
    }

    xUIElement = aRequestedToolbar.m_xUIElement;
    if ( !xUIElement.is() )
        bMustCallCreate = true;

    bool bCreateOrShowToolbar = aRequestedToolbar.m_bVisible & !aRequestedToolbar.m_bMasterHide;
    uno::Reference< awt::XWindow2 > xContainerWindow( m_xContainerWindow, uno::UNO_QUERY );
    if ( xContainerWindow.is() && aRequestedToolbar.m_bFloating )
        bCreateOrShowToolbar = ( bCreateOrShowToolbar && xContainerWindow->isActive() );

    if ( bCreateOrShowToolbar )
    {
        if ( bMustCallCreate )
            bNotify = createToolbar( rResourceURL );
        else
            bNotify = showToolbar( rResourceURL );
    }

    return bNotify;
}

bool ToolbarLayoutManager::createToolbar( const ::rtl::OUString& rResourceURL )
{
    bool bNotify( false );
    uno::Reference< ui::XUIElement > xUITempElement;

    implts_createToolBar( rResourceURL, bNotify, xUITempElement );
    return bNotify;
}

bool ToolbarLayoutManager::destroyToolbar( const ::rtl::OUString& rResourceURL )
{
    UIElementVector::iterator pIter;
    uno::Reference< lang::XComponent > xComponent;

    bool bMustBeDestroyed( false );
    bool bMustBeSorted( false );
    bool bNotify( false );
    bool bMustLayouted( false );
    rtl::OUString aAddonTbResourceName( RTL_CONSTASCII_USTRINGPARAM( "private:resource/toolbar/addon_" ));

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    for ( pIter = m_aUIElements.begin(); pIter != m_aUIElements.end(); pIter++ )
    {
        if ( pIter->m_aName == rResourceURL )
        {
            xComponent.set( pIter->m_xUIElement, uno::UNO_QUERY );
            uno::Reference< ui::XUIElement > xUIElement( pIter->m_xUIElement );
            if ( xUIElement.is() )
            {
                uno::Reference< awt::XWindow > xWindow( xUIElement->getRealInterface(), uno::UNO_QUERY );
                uno::Reference< awt::XDockableWindow > xDockWindow( xWindow, uno::UNO_QUERY );

                if ( rResourceURL.indexOf( aAddonTbResourceName ) != 0 )
                {
                    try
                    {
                        if ( xWindow.is() )
                            xWindow->removeWindowListener( uno::Reference< awt::XWindowListener >(
                                static_cast< OWeakObject * >( this ), uno::UNO_QUERY ));
                    }
                    catch( uno::Exception& )
                    {
                    }

                    try
                    {
                        if ( xDockWindow.is() )
                            xDockWindow->removeDockableWindowListener( uno::Reference< awt::XDockableWindowListener >(
                                static_cast< OWeakObject * >( this ), uno::UNO_QUERY ));
                    }
                    catch ( uno::Exception& )
                    {
                    }

                    bMustBeDestroyed = true;
                }
                else
                {
                    pIter->m_bVisible = sal_False;
                    xWindow->setVisible( sal_False );
                    bNotify = true;
                }

                if ( !xDockWindow->isFloating() )
                    bMustLayouted = true;
                if ( bMustBeDestroyed )
                    pIter->m_xUIElement.clear();

                bMustBeSorted = true;
            }
            break;
        }
    }
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    if ( bMustBeDestroyed )
    {
        if ( xComponent.is() )
            xComponent->dispose();
        bNotify = true;
    }

    if ( bMustLayouted )
    {
        aWriteLock.lock();
        m_bLayoutDirty = true;
        aWriteLock.unlock();
    }

    if ( bMustBeSorted )
        implts_sortUIElements();

    return bNotify;
}

void ToolbarLayoutManager::destroyToolbars()
{
    UIElementVector aUIElementVector;

    WriteGuard aWriteLock( m_aLock );
    aUIElementVector = m_aUIElements;
    m_aUIElements.clear();
    m_bLayoutDirty = true;
    aWriteLock.unlock();

    UIElementVector::iterator pIter;
    for ( pIter = aUIElementVector.begin(); pIter != aUIElementVector.end(); pIter++ )
    {
        uno::Reference< lang::XComponent > xComponent( pIter->m_xUIElement, uno::UNO_QUERY );
        if ( xComponent.is() )
            xComponent->dispose();
    }
}

bool ToolbarLayoutManager::showToolbar( const ::rtl::OUString& rResourceURL )
{
    UIElement aUIElement = implts_findToolbar( rResourceURL );

    vos::OGuard aGuard( Application::GetSolarMutex() );
    Window* pWindow = getWindowFromXUIElement( aUIElement.m_xUIElement );
    if ( pWindow )
    {
        pWindow->Show( TRUE, SHOW_NOFOCUSCHANGE | SHOW_NOACTIVATE );
        if ( !aUIElement.m_bFloating )
        {
            WriteGuard aWriteLock( m_aLock );
            m_bLayoutDirty = true;
        }

        aUIElement.m_bVisible = true;
        implts_writeWindowStateData( aUIElement );
        return true;
    }

    return false;
}

bool ToolbarLayoutManager::hideToolbar( const ::rtl::OUString& rResourceURL )
{
    UIElement aUIElement = implts_findToolbar( rResourceURL );

    vos::OGuard aGuard( Application::GetSolarMutex() );
    Window* pWindow = getWindowFromXUIElement( aUIElement.m_xUIElement );
    if ( pWindow )
    {
        pWindow->Show( FALSE );
        if ( !aUIElement.m_bFloating )
        {
            WriteGuard aWriteLock( m_aLock );
            m_bLayoutDirty = true;
        }

        aUIElement.m_bVisible = false;
        implts_writeWindowStateData( aUIElement );
        return true;
    }

    return false;
}

void ToolbarLayoutManager::refreshToolbarsVisibility()
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    UIElementVector aUIElementVector = m_aUIElements;
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    bool bLayoutDirty( false );
    UIElementVector::iterator pIter;

    vos::OGuard aGuard( Application::GetSolarMutex() );
    for ( pIter = aUIElementVector.begin(); pIter != aUIElementVector.end(); pIter++ )
    {
        Window* pWindow = getWindowFromXUIElement( pIter->m_xUIElement );
        if ( pWindow )
        {
            if ( pIter->m_bVisible && !pIter->m_bMasterHide )
                pWindow->Show( TRUE, SHOW_NOFOCUSCHANGE | SHOW_NOACTIVATE );
            else
                pWindow->Show( FALSE );

            if ( !pIter->m_bFloating )
                bLayoutDirty = true;
        }
    }
}

void ToolbarLayoutManager::setFloatingToolbarsVisibility( bool bVisible )
{
    UIElementVector aUIElementVector;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    aUIElementVector = m_aUIElements;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    UIElementVector::iterator pIter;
    vos::OGuard aGuard( Application::GetSolarMutex() );
    for ( pIter = aUIElementVector.begin(); pIter != aUIElementVector.end(); pIter++ )
    {
        Window* pWindow = getWindowFromXUIElement( pIter->m_xUIElement );
        if ( pWindow && pIter->m_bFloating )
        {
            if ( bVisible )
            {
                if ( pIter->m_bVisible && !pIter->m_bMasterHide )
                    pWindow->Show( TRUE, SHOW_NOFOCUSCHANGE | SHOW_NOACTIVATE );
            }
            else
                pWindow->Show( FALSE );
        }
    }
}

void ToolbarLayoutManager::setVisible( bool bVisible )
{
    UIElementVector aUIElementVector;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    aUIElementVector = m_aUIElements;
    m_bLayoutDirty   = true;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    UIElementVector::iterator pIter;

    vos::OGuard aGuard( Application::GetSolarMutex() );
    for ( pIter = aUIElementVector.begin(); pIter != aUIElementVector.end(); pIter++ )
    {
        bool bToolbarVisible = pIter->m_bVisible;

        pIter->m_bMasterHide = !bVisible;
        Window* pWindow = getWindowFromXUIElement( pIter->m_xUIElement );
        if ( pWindow )
            pWindow->Show(bVisible & bToolbarVisible, SHOW_NOFOCUSCHANGE | SHOW_NOACTIVATE );
    }

    if ( !bVisible )
        resetDockingArea();
}

bool ToolbarLayoutManager::dockToolbar(
    const ::rtl::OUString& /*rResourceURL*/,
    ::com::sun::star::ui::DockingArea /*eDockingArea*/,
    const ::com::sun::star::awt::Point& /*aPos*/ )
{
    return false;
}

bool ToolbarLayoutManager::dockAllToolbars()
{
    std::vector< ::rtl::OUString > aToolBarNameVector;

    ::rtl::OUString           aElementType;
    ::rtl::OUString           aElementName;
    UIElementVector::iterator pIter;

    ReadGuard aReadLock( m_aLock );
    for ( pIter = m_aUIElements.begin(); pIter != m_aUIElements.end(); pIter++ )
    {
        if ( pIter->m_aType.equalsAscii( "toolbar" ) &&
             pIter->m_xUIElement.is() &&
             pIter->m_bFloating &&
             pIter->m_bVisible )
            aToolBarNameVector.push_back( pIter->m_aName );
    }
    aReadLock.unlock();

    bool bResult(true);
    const sal_uInt32 nCount = aToolBarNameVector.size();
    for ( sal_uInt32 i = 0; i < nCount; ++i )
    {
        awt::Point aPoint;
        aPoint.X = aPoint.Y = SAL_MAX_INT32;
        bResult &= dockToolbar( aToolBarNameVector[i], ui::DockingArea_DOCKINGAREA_DEFAULT, aPoint );
    }

    return bResult;
}

long ToolbarLayoutManager::childWindowEvent( VclSimpleEvent* pEvent )
{
    // To enable toolbar controllers to change their image when a sub-toolbar function
    // is activated, we need this mechanism. We have NO connection between these toolbars
    // anymore!
    if ( pEvent && pEvent->ISA( VclWindowEvent ))
    {
        if ( pEvent->GetId() == VCLEVENT_TOOLBOX_SELECT )
        {
        ::rtl::OUString aToolbarName;
        ::rtl::OUString aCommand;
            ToolBox*        pToolBox = getToolboxPtr( ((VclWindowEvent*)pEvent)->GetWindow() );

            if ( pToolBox )
        {
            aToolbarName = retrieveToolbarNameFromHelpURL( pToolBox );
                USHORT nId = pToolBox->GetCurItemId();
                if ( nId > 0 )
                    aCommand = pToolBox->GetItemCommand( nId );
            }

            if (( aToolbarName.getLength() > 0 ) && ( aCommand.getLength() > 0 ))
            {
                /* SAFE AREA ----------------------------------------------------------------------------------------------- */
                ReadGuard aReadLock( m_aLock );
                ::std::vector< uno::Reference< ui::XUIFunctionListener > > aListenerArray;
                UIElementVector::iterator pIter;

                for ( pIter = m_aUIElements.begin(); pIter != m_aUIElements.end(); pIter++ )
                {
                    if ( pIter->m_aType.equalsAscii( "toolbar" ) && pIter->m_xUIElement.is() )
                    {
                uno::Reference< ui::XUIFunctionListener > xListener( pIter->m_xUIElement, uno::UNO_QUERY );
                        if ( xListener.is() )
                            aListenerArray.push_back( xListener );
                    }
                }
                aReadLock.unlock();
            /* SAFE AREA ----------------------------------------------------------------------------------------------- */

                const sal_uInt32 nCount = aListenerArray.size();
                for ( sal_uInt32 i = 0; i < nCount; ++i )
                {
                    try
                    {
                        aListenerArray[i]->functionExecute( aToolbarName, aCommand );
                    }
                    catch ( uno::RuntimeException& e )
                    {
                        throw e;
                    }
                    catch ( uno::Exception& ) {}
                }
            }
        }
        else if ( pEvent->GetId() == VCLEVENT_TOOLBOX_FORMATCHANGED )
        {
            ToolBox* pToolBox = getToolboxPtr( ((VclWindowEvent*)pEvent)->GetWindow() );

            if ( pToolBox )
        {
            ::rtl::OUString aToolbarName = retrieveToolbarNameFromHelpURL( pToolBox );

            if ( aToolbarName.getLength() > 0 )
        {
                    WriteGuard aWriteLock( m_aLock );
            m_bLayoutDirty = true;
                    aWriteLock.unlock();
            m_pParentLayouter->requestLayout( ILayoutNotifications::HINT_TOOLBARSPACE_HAS_CHANGED );
        }
            }
        }
    }

    return 1;
}

void ToolbarLayoutManager::resetDockingArea()
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    uno::Reference< awt::XWindow > xTopDockingWindow = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_TOP];
    uno::Reference< awt::XWindow > xLeftDockingWindow = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_LEFT];
    uno::Reference< awt::XWindow > xRightDockingWindow = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_RIGHT];
    uno::Reference< awt::XWindow > xBottomDockingWindow = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_BOTTOM];
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    if ( xTopDockingWindow.is() )
        xTopDockingWindow->setPosSize( 0, 0, 0, 0, awt::PosSize::POSSIZE );
    if ( xLeftDockingWindow.is() )
        xLeftDockingWindow->setPosSize( 0, 0, 0, 0, awt::PosSize::POSSIZE );
    if ( xRightDockingWindow.is() )
        xRightDockingWindow->setPosSize( 0, 0, 0, 0, awt::PosSize::POSSIZE );
    if ( xBottomDockingWindow.is() )
        xBottomDockingWindow->setPosSize( 0, 0, 0, 0, awt::PosSize::POSSIZE );
}

void ToolbarLayoutManager::setParentWindow(
    const uno::Reference< awt::XWindowPeer >& xParentWindow )
{
    static const char DOCKINGAREASTRING[] = "dockingarea";

    uno::Reference< awt::XWindow > xTopDockingWindow = uno::Reference< awt::XWindow >( createToolkitWindow( m_xSMGR, xParentWindow, DOCKINGAREASTRING ), uno::UNO_QUERY );
    uno::Reference< awt::XWindow > xLeftDockingWindow = uno::Reference< awt::XWindow >( createToolkitWindow( m_xSMGR, xParentWindow, DOCKINGAREASTRING ), uno::UNO_QUERY );
    uno::Reference< awt::XWindow > xRightDockingWindow = uno::Reference< awt::XWindow >( createToolkitWindow( m_xSMGR, xParentWindow, DOCKINGAREASTRING ), uno::UNO_QUERY );
    uno::Reference< awt::XWindow > xBottomDockingWindow = uno::Reference< awt::XWindow >( createToolkitWindow( m_xSMGR, xParentWindow, DOCKINGAREASTRING ), uno::UNO_QUERY );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    m_xContainerWindow = uno::Reference< awt::XWindow2 >( xParentWindow, uno::UNO_QUERY );

    m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_TOP]    = xTopDockingWindow;
    m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_LEFT]   = xLeftDockingWindow;
    m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_RIGHT]  = xRightDockingWindow;
    m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_BOTTOM] = xBottomDockingWindow;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    if ( xParentWindow.is() )
        implts_reparentToolbars();
    else
    {
        destroyToolbars();
        resetDockingArea();
    }
}

void ToolbarLayoutManager::setDockingAreaOffsets( const ::Rectangle aOffsets )
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    m_aDockingAreaOffsets = aOffsets;
    m_bLayoutDirty        = true;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
}

rtl::OUString ToolbarLayoutManager::implts_generateGenericAddonToolbarTitle( sal_Int32 nNumber ) const
{
    String aAddonGenericTitle;

    aAddonGenericTitle = String( FwkResId( STR_TOOLBAR_TITLE_ADDON ));
    const vcl::I18nHelper& rI18nHelper = Application::GetSettings().GetUILocaleI18nHelper();

    String aNumStr = rI18nHelper.GetNum( nNumber, 0, FALSE, FALSE );
    aAddonGenericTitle.SearchAndReplaceAscii( "%num%", aNumStr );

    return rtl::OUString( aAddonGenericTitle );
}

void ToolbarLayoutManager::implts_createAddonsToolBars()
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    if ( !m_pAddonOptions )
        m_pAddonOptions = new AddonsOptions;

    uno::Reference< ui::XUIElementFactory > xUIElementFactory( m_xUIElementFactoryManager );
    uno::Reference< frame::XFrame > xFrame( m_xFrame );
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    uno::Reference< frame::XModel > xModel( impl_getModelFromFrame( xFrame ));
    if ( implts_isPreviewModel( xModel ))
        return; // no addon toolbars for preview frame!

    UIElementVector aUIElementVector;
    uno::Sequence< uno::Sequence< beans::PropertyValue > > aAddonToolBarData;
    uno::Reference< ui::XUIElement >                       xUIElement;

    sal_uInt32 nCount = m_pAddonOptions->GetAddonsToolBarCount();
    ::rtl::OUString aAddonsToolBarStaticName( m_aFullAddonTbxPrefix );
    ::rtl::OUString aElementType( RTL_CONSTASCII_USTRINGPARAM( "toolbar" ));

    uno::Sequence< beans::PropertyValue > aPropSeq( 2 );
    aPropSeq[0].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Frame" ));
    aPropSeq[0].Value <<= xFrame;
    aPropSeq[1].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ConfigurationData" ));
    for ( sal_uInt32 i = 0; i < nCount; i++ )
    {
        ::rtl::OUString aAddonToolBarName( aAddonsToolBarStaticName + m_pAddonOptions->GetAddonsToolbarResourceName(i) );
        aAddonToolBarData = m_pAddonOptions->GetAddonsToolBarPart( i );
        aPropSeq[1].Value <<= aAddonToolBarData;

        UIElement aElement = implts_findToolbar( aAddonToolBarName );

        // #i79828
        // It's now possible that we are called more than once. Be sure to not create
        // add-on toolbars more than once!
        if ( aElement.m_xUIElement.is() )
            continue;

        try
        {
            xUIElement = xUIElementFactory->createUIElement( aAddonToolBarName, aPropSeq );
            if ( xUIElement.is() )
            {
                uno::Reference< awt::XDockableWindow > xDockWindow( xUIElement->getRealInterface(), uno::UNO_QUERY );
                if ( xDockWindow.is() )
                {
                    try
                    {
                        xDockWindow->addDockableWindowListener( uno::Reference< awt::XDockableWindowListener >( static_cast< OWeakObject * >( this ), uno::UNO_QUERY ));
                        xDockWindow->enableDocking( sal_True );
                        uno::Reference< awt::XWindow > xWindow( xDockWindow, uno::UNO_QUERY );
                        if ( xWindow.is() )
                            xWindow->addWindowListener( uno::Reference< awt::XWindowListener >( static_cast< OWeakObject * >( this ), uno::UNO_QUERY ));
                    }
                    catch ( uno::Exception& )
                    {
                    }
                }

                ::rtl::OUString aGenericAddonTitle = implts_generateGenericAddonToolbarTitle( i+1 );

                if ( aElement.m_aName.getLength() > 0 )
                {
                    // Reuse a local entry so we are able to use the latest
                    // UI changes for this document.
                    implts_setElementData( aElement, xDockWindow );
                    aElement.m_xUIElement = xUIElement;
                    if ( aElement.m_aUIName.getLength() == 0 )
                    {
                        aElement.m_aUIName = aGenericAddonTitle;
                        implts_writeWindowStateData( aElement );
                    }
                }
                else
                {
                    // Create new UI element and try to read its state data
                    UIElement aNewToolbar( aAddonToolBarName, aElementType, xUIElement );
                    aNewToolbar.m_bFloating = sal_True;
                    implts_readWindowStateData( aAddonToolBarName, aNewToolbar );
                    implts_setElementData( aNewToolbar, xDockWindow );
                    if ( aNewToolbar.m_aUIName.getLength() == 0 )
                    {
                        aNewToolbar.m_aUIName = aGenericAddonTitle;
                        implts_writeWindowStateData( aNewToolbar );
                    }
                    implts_insertToolbar( aNewToolbar );
                }

                uno::Reference< awt::XWindow > xWindow( xDockWindow, uno::UNO_QUERY );
                if ( xWindow.is() )
                {
                    // Set generic title for add-on toolbar
                    vos::OGuard aGuard( Application::GetSolarMutex() );
                    Window* pWindow = VCLUnoHelper::GetWindow( xWindow );
                    if ( pWindow->GetText().Len() == 0 )
                        pWindow->SetText( aGenericAddonTitle );
                    if ( pWindow->GetType() == WINDOW_TOOLBOX )
                    {
                        ToolBox* pToolbar = (ToolBox *)pWindow;
                        pToolbar->SetMenuType();
                    }
                }
            }
        }
        catch ( container::NoSuchElementException& )
        {
        }
        catch ( lang::IllegalArgumentException& )
        {
        }
    }
}

void ToolbarLayoutManager::implts_createCustomToolBars()
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    if ( !m_bComponentAttached )
        return;

    uno::Reference< ui::XUIElementFactory > xUIElementFactory( m_xUIElementFactoryManager );
    uno::Reference< frame::XFrame > xFrame( m_xFrame );
    uno::Reference< frame::XModel > xModel;
    uno::Reference< ui::XUIConfigurationManager > xModuleCfgMgr( m_xModuleCfgMgr, uno::UNO_QUERY );
    uno::Reference< ui::XUIConfigurationManager > xDocCfgMgr( m_xDocCfgMgr, uno::UNO_QUERY );
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    if ( xFrame.is() )
    {
        xModel = impl_getModelFromFrame( xFrame );
        if ( implts_isPreviewModel( xModel ))
            return; // no custom toolbars for preview frame!

        uno::Sequence< uno::Sequence< beans::PropertyValue > > aTbxSeq;
        if ( xDocCfgMgr.is() )
        {
            aTbxSeq = xDocCfgMgr->getUIElementsInfo( ui::UIElementType::TOOLBAR );
            implts_createCustomToolBars( aTbxSeq ); // first create all document based toolbars
        }
        if ( xModuleCfgMgr.is() )
        {
            aTbxSeq = xModuleCfgMgr->getUIElementsInfo( ui::UIElementType::TOOLBAR );
            implts_createCustomToolBars( aTbxSeq ); // second create module based toolbars
        }
    }
}

void ToolbarLayoutManager::implts_createNonContextSensitiveToolBars()
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );

    if ( !m_xPersistentWindowState.is() || !m_xFrame.is() || !m_bComponentAttached )
        return;

    uno::Reference< frame::XFrame >          xFrame( m_xFrame );
    uno::Reference< ui::XUIElementFactory >  xUIElementFactory( m_xUIElementFactoryManager );
    uno::Reference< container::XNameAccess > xPersistentWindowState( m_xPersistentWindowState );
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    if ( implts_isPreviewModel( impl_getModelFromFrame( xFrame )))
        return;

    std::vector< rtl::OUString > aMakeVisibleToolbars;

    try
    {
        uno::Sequence< ::rtl::OUString > aToolbarNames = xPersistentWindowState->getElementNames();

        if ( aToolbarNames.getLength() > 0 )
        {
            ::rtl::OUString aElementType;
            ::rtl::OUString aElementName;
            ::rtl::OUString aName;

            uno::Reference< ui::XUIElement > xUIElement;
            aMakeVisibleToolbars.reserve(aToolbarNames.getLength());

            /* SAFE AREA ----------------------------------------------------------------------------------------------- */
            WriteGuard aWriteLock( m_aLock );

            const rtl::OUString* pTbNames = aToolbarNames.getConstArray();
            for ( sal_Int32 i = 0; i < aToolbarNames.getLength(); i++ )
            {
                aName = pTbNames[i];
                parseResourceURL( aName, aElementType, aElementName );

                // Check that we only create:
                // - Toolbars (the statusbar is also member of the persistent window state)
                // - Not custom toolbars, there are created with their own method (implts_createCustomToolbars)
                if ( aElementType.equalsIgnoreAsciiCaseAscii( "toolbar" ) && aElementName.indexOf( m_aCustomTbxPrefix ) == -1 )
                {
                    UIElement aNewToolbar = implts_findToolbar( aName );
                    bool bFound = ( aNewToolbar.m_aName == aName );
                    if ( !bFound )
                        implts_readWindowStateData( aName, aNewToolbar );

                    if ( aNewToolbar.m_bVisible && !aNewToolbar.m_bContextSensitive )
                    {
                        if ( !bFound )
                            implts_insertToolbar( aNewToolbar );
                        aMakeVisibleToolbars.push_back( aName );
                    }
                }
            }
            /* SAFE AREA ----------------------------------------------------------------------------------------------- */
        }
    }
    catch ( uno::RuntimeException& e )
    {
        throw e;
    }
    catch ( uno::Exception& )
    {
    }

    if ( !aMakeVisibleToolbars.empty() )
        ::std::for_each( aMakeVisibleToolbars.begin(), aMakeVisibleToolbars.end(),::boost::bind( &ToolbarLayoutManager::requestToolbar, this,_1 ));
}

void ToolbarLayoutManager::implts_createCustomToolBars( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > >& aTbxSeqSeq )
{
    const uno::Sequence< beans::PropertyValue >* pTbxSeq = aTbxSeqSeq.getConstArray();
    for ( sal_Int32 i = 0; i < aTbxSeqSeq.getLength(); i++ )
    {
        const uno::Sequence< beans::PropertyValue >& rTbxSeq = pTbxSeq[i];
        ::rtl::OUString aTbxResName;
        ::rtl::OUString aTbxTitle;
        for ( sal_Int32 j = 0; j < rTbxSeq.getLength(); j++ )
        {
            if ( rTbxSeq[j].Name.equalsAscii( "ResourceURL" ))
                rTbxSeq[j].Value >>= aTbxResName;
            else if ( rTbxSeq[j].Name.equalsAscii( "UIName" ))
                rTbxSeq[j].Value >>= aTbxTitle;
        }

        // Only create custom toolbars. Their name have to start with "custom_"!
        if ( aTbxResName.getLength() > 0 && aTbxResName.indexOf( m_aCustomTbxPrefix ) != -1 )
            implts_createCustomToolBar( aTbxResName, aTbxTitle );
    }
}

void ToolbarLayoutManager::implts_createCustomToolBar( const rtl::OUString& aTbxResName, const rtl::OUString& aTitle )
{
    if ( aTbxResName.getLength() > 0 )
    {
        bool bNotify( false );
        uno::Reference< ui::XUIElement > xUIElement;
        implts_createToolBar( aTbxResName, bNotify, xUIElement );

        if ( aTitle && xUIElement.is() )
        {
            vos::OGuard aGuard( Application::GetSolarMutex() );

            Window* pWindow = getWindowFromXUIElement( xUIElement );
            if ( pWindow  )
                pWindow->SetText( aTitle );
        }
    }
}

void ToolbarLayoutManager::implts_reparentToolbars()
{
    UIElementVector aUIElementVector;
    uno::Reference< awt::XWindow > xTopDockWindow;
    uno::Reference< awt::XWindow > xBottomDockWindow;
    uno::Reference< awt::XWindow > xLeftDockWindow;
    uno::Reference< awt::XWindow > xRightDockWindow;
    uno::Reference< awt::XWindow2 > xContainerWindow;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    aUIElementVector    = m_aUIElements;
    xTopDockWindow      = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_TOP];
    xBottomDockWindow   = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_BOTTOM];
    xLeftDockWindow     = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_LEFT];
    xRightDockWindow    = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_RIGHT];
    xContainerWindow    = m_xContainerWindow;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    vos::OGuard aGuard( Application::GetSolarMutex() );
    Window* pContainerWindow    = VCLUnoHelper::GetWindow( xContainerWindow );
    Window* pTopDockWindow      = VCLUnoHelper::GetWindow( xTopDockWindow );
    Window* pBottomDockWindow   = VCLUnoHelper::GetWindow( xBottomDockWindow );
    Window* pLeftDockWindow     = VCLUnoHelper::GetWindow( xLeftDockWindow );
    Window* pRightDockWindow    = VCLUnoHelper::GetWindow( xRightDockWindow );

    if ( pContainerWindow )
    {
        UIElementVector::iterator pIter;
        for ( pIter = aUIElementVector.begin(); pIter != aUIElementVector.end(); pIter++ )
        {
            uno::Reference< ui::XUIElement > xUIElement( pIter->m_xUIElement );
            if ( xUIElement.is() )
            {
                uno::Reference< awt::XWindow > xWindow;
                try
                {
                    // We have to retreive the window reference with try/catch as it is
                    // possible that all elements have been disposed!
                    xWindow = uno::Reference< awt::XWindow >( xUIElement->getRealInterface(), uno::UNO_QUERY );
                }
                catch ( uno::RuntimeException& )
                {
                    throw;
                }
                catch ( uno::Exception& )
                {
                }

                Window* pWindow = VCLUnoHelper::GetWindow( xWindow );
                if ( pWindow )
                {
                    // Reparent our child windows acording to their current state.
                    if ( pIter->m_bFloating )
                        pWindow->SetParent( pContainerWindow );
                    else
                    {
                        if ( pIter->m_aDockedData.m_nDockedArea == ui::DockingArea_DOCKINGAREA_TOP )
                            pWindow->SetParent( pTopDockWindow );
                        else if ( pIter->m_aDockedData.m_nDockedArea == ui::DockingArea_DOCKINGAREA_BOTTOM )
                            pWindow->SetParent( pBottomDockWindow );
                        else if ( pIter->m_aDockedData.m_nDockedArea == ui::DockingArea_DOCKINGAREA_LEFT )
                            pWindow->SetParent( pLeftDockWindow );
                        else
                            pWindow->SetParent( pRightDockWindow );
                    }
                }
            }
        }
    }
}

void ToolbarLayoutManager::implts_createToolBar( const ::rtl::OUString& aName, bool& bNotify, uno::Reference< ui::XUIElement >& rUIElement )
{
    bNotify = false;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    uno::Reference< frame::XFrame > xFrame( m_xFrame );
    uno::Reference< awt::XWindow2 > xContainerWindow( m_xContainerWindow );
    uno::Reference< ui::XUIElementFactory > xUIElementFactory( m_xUIElementFactoryManager );
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    if ( !xFrame.is() || !xContainerWindow.is() )
        return;

    UIElement aToolbarElement = implts_findToolbar( aName );
    if ( !aToolbarElement.m_xUIElement.is()  )
    {
        uno::Reference< ui::XUIElement > xUIElement = implts_createElement( aName );
        sal_Bool bVisible( sal_False );
        if ( xUIElement.is() )
        {
            rUIElement = xUIElement;

            uno::Reference< awt::XWindow > xWindow( xUIElement->getRealInterface(), uno::UNO_QUERY );
            uno::Reference< awt::XDockableWindow > xDockWindow( xWindow, uno::UNO_QUERY );
            if ( xDockWindow.is() && xWindow.is() )
            {
                try
                {
                    xDockWindow->addDockableWindowListener( uno::Reference< awt::XDockableWindowListener >(
                        static_cast< OWeakObject * >( this ), uno::UNO_QUERY ));
                    xWindow->addWindowListener( uno::Reference< awt::XWindowListener >(
                        static_cast< OWeakObject * >( this ), uno::UNO_QUERY ));
                    xDockWindow->enableDocking( sal_True );
                }
                catch ( uno::Exception& )
                {
                }
            }

            /* SAFE AREA ----------------------------------------------------------------------------------------------- */
            WriteGuard aWriteLock( m_aLock );

            UIElement& rElement = impl_findToolbar( aName );
            if ( rElement.m_aName.getLength() > 0 )
            {
                // Reuse a local entry so we are able to use the latest
                // UI changes for this document.
                implts_setElementData( rElement, xDockWindow );
                rElement.m_xUIElement = xUIElement;
                bVisible = rElement.m_bVisible;
            }
            else
            {
                // Create new UI element and try to read its state data
                UIElement aNewToolbar( aName, m_aToolbarTypeString, xUIElement );
                implts_readWindowStateData( aName, aNewToolbar );
                implts_setElementData( aNewToolbar, xDockWindow );
                implts_insertToolbar( aNewToolbar );
                bVisible = aNewToolbar.m_bVisible;
            }

            // set toolbar menu style according to customize command state
            SvtCommandOptions aCmdOptions;

            Window* pWindow = VCLUnoHelper::GetWindow( xWindow );
            if ( pWindow && pWindow->GetType() == WINDOW_TOOLBOX )
            {
                ToolBox* pToolbar = (ToolBox *)pWindow;
                USHORT nMenuType = pToolbar->GetMenuType();
                if ( aCmdOptions.Lookup( SvtCommandOptions::CMDOPTION_DISABLED, m_aCustomizeCmd ))
                    pToolbar->SetMenuType( nMenuType & ~TOOLBOX_MENUTYPE_CUSTOMIZE );
                else
                    pToolbar->SetMenuType( nMenuType | TOOLBOX_MENUTYPE_CUSTOMIZE );
            }

            aWriteLock.unlock();
            /* SAFE AREA ----------------------------------------------------------------------------------------------- */
        }

        implts_sortUIElements();

        if ( bVisible )
        {
            /* SAFE AREA ----------------------------------------------------------------------------------------------- */
            WriteGuard aWriteLock( m_aLock );
            m_bLayoutDirty = true;
            bNotify        = true;
            /* SAFE AREA ----------------------------------------------------------------------------------------------- */
        }
    }
}

uno::Reference< ui::XUIElement > ToolbarLayoutManager::implts_createElement( const ::rtl::OUString& aName )
{
    uno::Reference< ui::XUIElement > xUIElement;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard   aReadLock( m_aLock );
    uno::Sequence< beans::PropertyValue > aPropSeq( 2 );
    aPropSeq[0].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Frame" ));
    aPropSeq[0].Value <<= m_xFrame;
    aPropSeq[1].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Persistent" ));
    aPropSeq[1].Value <<= sal_True;

    try
    {
        if ( m_xUIElementFactoryManager.is() )
            xUIElement = m_xUIElementFactoryManager->createUIElement( aName, aPropSeq );
    }
    catch ( container::NoSuchElementException& )
    {
    }
    catch ( lang::IllegalArgumentException& )
    {
    }

    return xUIElement;
}

void ToolbarLayoutManager::implts_setElementData( UIElement& rElement, const uno::Reference< awt::XDockableWindow >& rDockWindow )
{
    ReadGuard aReadLock( m_aLock );
    sal_Bool bShowElement( rElement.m_bVisible && !rElement.m_bMasterHide && implts_isParentWindowVisible() );
    aReadLock.unlock();

    uno::Reference< awt::XDockableWindow > xDockWindow( rDockWindow );
    uno::Reference< awt::XWindow2 >        xWindow( xDockWindow, uno::UNO_QUERY );

    Window*     pWindow( 0 );
    ToolBox*    pToolBox( 0 );

    if ( xDockWindow.is() && xWindow.is() )
    {
        {
            vos::OGuard aGuard( Application::GetSolarMutex() );
            pWindow = VCLUnoHelper::GetWindow( xWindow );
            if ( pWindow )
            {
                String aText = pWindow->GetText();
                if ( aText.Len() == 0 )
                    pWindow->SetText( rElement.m_aUIName );
                if ( rElement.m_bNoClose )
                    pWindow->SetStyle( pWindow->GetStyle() & ~WB_CLOSEABLE );
                if ( pWindow->GetType() == WINDOW_TOOLBOX )
                    pToolBox = (ToolBox *)pWindow;
            }
            if ( pToolBox )
            {
                if (( rElement.m_nStyle < 0 ) ||
                    ( rElement.m_nStyle > BUTTON_SYMBOLTEXT ))
                    rElement.m_nStyle = BUTTON_SYMBOL;
                    pToolBox->SetButtonType( (ButtonType)rElement.m_nStyle );
                if ( rElement.m_bNoClose )
                    pToolBox->SetFloatStyle( pToolBox->GetFloatStyle() & ~WB_CLOSEABLE );
            }
        }

        if ( rElement.m_bFloating )
        {
            if ( pWindow )
            {
                vos::OGuard aGuard( Application::GetSolarMutex() );
                String aText = pWindow->GetText();
                if ( aText.Len() == 0 )
                    pWindow->SetText( rElement.m_aUIName );
            }

            ::Point  aPos( rElement.m_aFloatingData.m_aPos.X(),
                           rElement.m_aFloatingData.m_aPos.Y() );
            sal_Bool bWriteData( sal_False );
            sal_Bool bUndefPos = ( rElement.m_aFloatingData.m_aPos.X() == SAL_MAX_INT32 ||
                                   rElement.m_aFloatingData.m_aPos.Y() == SAL_MAX_INT32 );
            sal_Bool bSetSize = ( rElement.m_aFloatingData.m_aSize.Width() != 0 &&
                                  rElement.m_aFloatingData.m_aSize.Height() != 0 );
            xDockWindow->setFloatingMode( sal_True );
            if ( bUndefPos )
            {
                aPos = implts_findNextCascadeFloatingPos();
                rElement.m_aFloatingData.m_aPos = aPos; // set new cascaded position
                bWriteData = sal_True;
            }

            if( bSetSize )
                xWindow->setOutputSize( AWTSize( rElement.m_aFloatingData.m_aSize ) );
            else
            {
                if( pToolBox )
                {
                    // set an optimal initial floating size
                    vos::OGuard       aGuard( Application::GetSolarMutex() );
                    ::Size aSize( pToolBox->CalcFloatingWindowSizePixel() );
                    pToolBox->SetOutputSizePixel( aSize );
                }
            }

            // #i60882# IMPORTANT: Set position after size as it is
            // possible that we position some part of the toolbar
            // outside of the desktop. A default constructed toolbar
            // always has one line. Now VCL automatically
            // position the toolbar back into the desktop. Therefore
            // we resize the toolbar with the new (wrong) position.
            // To fix this problem we have to set the size BEFORE the
            // position.
            xWindow->setPosSize( aPos.X(), aPos.Y(), 0, 0,
                                 css::awt::PosSize::POS );

            if ( bWriteData )
                implts_writeWindowStateData( rElement );
            if ( bShowElement && pWindow )
            {
                vos::OGuard aGuard( Application::GetSolarMutex() );
                pWindow->Show( sal_True, SHOW_NOFOCUSCHANGE | SHOW_NOACTIVATE );
            }
        }
        else
        {
            ::Point  aDockPos;
            ::Point  aPixelPos;
            sal_Bool bSetSize( sal_False );
            ::Size   aSize;

            if ( pToolBox )
            {
                vos::OGuard aGuard( Application::GetSolarMutex() );
                pToolBox->SetAlign( ImplConvertAlignment(rElement.m_aDockedData.m_nDockedArea )  );
                pToolBox->SetLineCount( 1 );
                if ( rElement.m_aDockedData.m_bLocked )
                    xDockWindow->lock();
                aSize = pToolBox->CalcWindowSizePixel();
                bSetSize = sal_True;

                if (( rElement.m_aDockedData.m_aPos.X() == SAL_MAX_INT32 ) &&
                    ( rElement.m_aDockedData.m_aPos.Y() == SAL_MAX_INT32 ))
                {
                    implts_findNextDockingPos( (ui::DockingArea)rElement.m_aDockedData.m_nDockedArea,
                                               aSize,
                                               aDockPos,
                                               aPixelPos );
                    rElement.m_aDockedData.m_aPos = aDockPos;
                }
            }

            xWindow->setPosSize( aPixelPos.X(),
                                 aPixelPos.Y(),
                                 0, 0,
                                 css::awt::PosSize::POS );
            if( bSetSize )
                xWindow->setOutputSize( AWTSize( aSize) );

            if ( bShowElement && pWindow )
            {
                vos::OGuard aGuard( Application::GetSolarMutex() );
                pWindow->Show( sal_True );
            }
        }
    }
}

void ToolbarLayoutManager::implts_destroyDockingAreaWindows()
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    uno::Reference< awt::XWindow > xTopDockingWindow    = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_TOP];
    uno::Reference< awt::XWindow > xLeftDockingWindow   = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_LEFT];
    uno::Reference< awt::XWindow > xRightDockingWindow  = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_RIGHT];
    uno::Reference< awt::XWindow > xBottomDockingWindow = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_BOTTOM];
    m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_TOP].clear();
    m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_LEFT].clear();
    m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_RIGHT].clear();
    m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_BOTTOM].clear();
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    // destroy windows
    xTopDockingWindow->dispose();
    xLeftDockingWindow->dispose();
    xRightDockingWindow->dispose();
    xBottomDockingWindow->dispose();
}

//---------------------------------------------------------------------------------------------------------
// persistence methods
//---------------------------------------------------------------------------------------------------------

sal_Bool ToolbarLayoutManager::implts_readWindowStateData( const rtl::OUString& aName, UIElement& rElementData )
{
    sal_Bool bGetSettingsState( sal_False );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    uno::Reference< container::XNameAccess > xPersistentWindowState( m_xPersistentWindowState );
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    if ( xPersistentWindowState.is() )
    {
        /* SAFE AREA ----------------------------------------------------------------------------------------------- */
        aWriteLock.lock();
        sal_Bool bGlobalSettings( m_bGlobalSettings );
        GlobalSettings* pGlobalSettings( 0 );
        if ( m_pGlobalSettings == 0 )
        {
            m_pGlobalSettings = new GlobalSettings( m_xSMGR );
            bGetSettingsState = sal_True;
        }
        pGlobalSettings = m_pGlobalSettings;
        aWriteLock.unlock();
        /* SAFE AREA ----------------------------------------------------------------------------------------------- */

        try
        {
            uno::Sequence< beans::PropertyValue > aWindowState;
            if ( xPersistentWindowState->getByName( aName ) >>= aWindowState )
            {
                sal_Bool bValue( sal_False );
                for ( sal_Int32 n = 0; n < aWindowState.getLength(); n++ )
                {
                    if ( aWindowState[n].Name.equalsAscii( WINDOWSTATE_PROPERTY_DOCKED ))
                    {
                        if ( aWindowState[n].Value >>= bValue )
                            rElementData.m_bFloating = !bValue;
                    }
                    else if ( aWindowState[n].Name.equalsAscii( WINDOWSTATE_PROPERTY_VISIBLE ))
                    {
                        if ( aWindowState[n].Value >>= bValue )
                            rElementData.m_bVisible = bValue;
                    }
                    else if ( aWindowState[n].Name.equalsAscii( WINDOWSTATE_PROPERTY_DOCKINGAREA ))
                    {
                        ui::DockingArea eDockingArea;
                        if ( aWindowState[n].Value >>= eDockingArea )
                            rElementData.m_aDockedData.m_nDockedArea = sal_Int16( eDockingArea );
                    }
                    else if ( aWindowState[n].Name.equalsAscii( WINDOWSTATE_PROPERTY_DOCKPOS ))
                    {
                        awt::Point aPoint;
                        if ( aWindowState[n].Value >>= aPoint )
                        {
                            rElementData.m_aDockedData.m_aPos.X() = aPoint.X;
                            rElementData.m_aDockedData.m_aPos.Y() = aPoint.Y;
                        }
                    }
                    else if ( aWindowState[n].Name.equalsAscii( WINDOWSTATE_PROPERTY_POS ))
                    {
                        awt::Point aPoint;
                        if ( aWindowState[n].Value >>= aPoint )
                        {
                            rElementData.m_aFloatingData.m_aPos.X() = aPoint.X;
                            rElementData.m_aFloatingData.m_aPos.Y() = aPoint.Y;
                        }
                    }
                    else if ( aWindowState[n].Name.equalsAscii( WINDOWSTATE_PROPERTY_SIZE ))
                    {
                        awt::Size aSize;
                        if ( aWindowState[n].Value >>= aSize )
                        {
                            rElementData.m_aFloatingData.m_aSize.Width() = aSize.Width;
                            rElementData.m_aFloatingData.m_aSize.Height() = aSize.Height;
                        }
                    }
                    else if ( aWindowState[n].Name.equalsAscii( WINDOWSTATE_PROPERTY_UINAME ))
                        aWindowState[n].Value >>= rElementData.m_aUIName;
                    else if ( aWindowState[n].Name.equalsAscii( WINDOWSTATE_PROPERTY_STYLE ))
                    {
                        sal_Int32 nStyle = 0;
                        if ( aWindowState[n].Value >>= nStyle )
                            rElementData.m_nStyle = sal_Int16( nStyle );
                    }
                    else if ( aWindowState[n].Name.equalsAscii( WINDOWSTATE_PROPERTY_LOCKED ))
                    {
                        if ( aWindowState[n].Value >>= bValue )
                            rElementData.m_aDockedData.m_bLocked = bValue;
                    }
                    else if ( aWindowState[n].Name.equalsAscii( WINDOWSTATE_PROPERTY_CONTEXT ))
                    {
                        if ( aWindowState[n].Value >>= bValue )
                            rElementData.m_bContextSensitive = bValue;
                    }
                    else if ( aWindowState[n].Name.equalsAscii( WINDOWSTATE_PROPERTY_NOCLOSE ))
                    {
                        if ( aWindowState[n].Value >>= bValue )
                            rElementData.m_bNoClose = bValue;
                    }
                    else if ( aWindowState[n].Name.equalsAscii( WINDOWSTATE_PROPERTY_CONTEXTACTIVE ))
                    {
                        if ( aWindowState[n].Value >>= bValue )
                            rElementData.m_bContextActive = bValue;
                    }
                    else if ( aWindowState[n].Name.equalsAscii( WINDOWSTATE_PROPERTY_SOFTCLOSE ))
                    {
                        if ( aWindowState[n].Value >>= bValue )
                            rElementData.m_bSoftClose = bValue;
                    }
                }
            }

            // oversteer values with global settings
            if ( pGlobalSettings && ( bGetSettingsState || bGlobalSettings ))
            {
                if ( pGlobalSettings->HasStatesInfo( GlobalSettings::UIELEMENT_TYPE_TOOLBAR ))
                {
                    WriteGuard aWriteLock2( m_aLock );
                    m_bGlobalSettings = sal_True;
                    aWriteLock2.unlock();

                    css::uno::Any aValue;
                    sal_Bool      bValue = sal_Bool();
                    if ( pGlobalSettings->GetStateInfo( GlobalSettings::UIELEMENT_TYPE_TOOLBAR,
                                                        GlobalSettings::STATEINFO_LOCKED,
                                                        aValue ))
                        aValue >>= rElementData.m_aDockedData.m_bLocked;
                    if ( pGlobalSettings->GetStateInfo( GlobalSettings::UIELEMENT_TYPE_TOOLBAR,
                                                        GlobalSettings::STATEINFO_DOCKED,
                                                        aValue ))
                    {
                        if ( aValue >>= bValue )
                            rElementData.m_bFloating = !bValue;
                    }
                }
            }

            return sal_True;
        }
        catch ( container::NoSuchElementException& )
        {
        }
    }

    return sal_False;
}

void ToolbarLayoutManager::implts_writeWindowStateData( const UIElement& rElementData )
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    uno::Reference< container::XNameAccess > xPersistentWindowState( m_xPersistentWindowState );

    // set flag to determine that we triggered the notification
    m_bStoreWindowState = true;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    sal_Bool bPersistent( sal_False );
    uno::Reference< beans::XPropertySet > xPropSet( rElementData.m_xUIElement, uno::UNO_QUERY );
    if ( xPropSet.is() )
    {
        try
        {
            // Check persistent flag of the user interface element
            xPropSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Persistent" ))) >>= bPersistent;
        }
        catch ( beans::UnknownPropertyException )
        {
            // Non-configurable elements should at least store their dimension/position
            bPersistent = sal_True;
        }
        catch ( lang::WrappedTargetException )
        {
        }
    }

    if ( bPersistent && xPersistentWindowState.is() )
    {
        try
        {
            uno::Sequence< beans::PropertyValue > aWindowState( 8 );

            aWindowState[0].Name  = ::rtl::OUString::createFromAscii( WINDOWSTATE_PROPERTY_DOCKED );
            aWindowState[0].Value = ::uno::makeAny( sal_Bool( !rElementData.m_bFloating ));
            aWindowState[1].Name  = ::rtl::OUString::createFromAscii( WINDOWSTATE_PROPERTY_VISIBLE );
            aWindowState[1].Value = uno::makeAny( sal_Bool( rElementData.m_bVisible ));

            aWindowState[2].Name  = ::rtl::OUString::createFromAscii( WINDOWSTATE_PROPERTY_DOCKINGAREA );
            aWindowState[2].Value = uno::makeAny( static_cast< ui::DockingArea >( rElementData.m_aDockedData.m_nDockedArea ) );

            awt::Point aPos;
            aPos.X = rElementData.m_aDockedData.m_aPos.X();
            aPos.Y = rElementData.m_aDockedData.m_aPos.Y();
            aWindowState[3].Name  = ::rtl::OUString::createFromAscii( WINDOWSTATE_PROPERTY_DOCKPOS );
            aWindowState[3].Value <<= aPos;

            aPos.X = rElementData.m_aFloatingData.m_aPos.X();
            aPos.Y = rElementData.m_aFloatingData.m_aPos.Y();
            aWindowState[4].Name  = ::rtl::OUString::createFromAscii( WINDOWSTATE_PROPERTY_POS );
            aWindowState[4].Value <<= aPos;

            awt::Size aSize;
            aSize.Width  = rElementData.m_aFloatingData.m_aSize.Width();
            aSize.Height = rElementData.m_aFloatingData.m_aSize.Height();
            aWindowState[5].Name  = ::rtl::OUString::createFromAscii( WINDOWSTATE_PROPERTY_SIZE );
            aWindowState[5].Value <<= aSize;
            aWindowState[6].Name  = ::rtl::OUString::createFromAscii( WINDOWSTATE_PROPERTY_UINAME );
            aWindowState[6].Value = uno::makeAny( rElementData.m_aUIName );
            aWindowState[7].Name  = ::rtl::OUString::createFromAscii( WINDOWSTATE_PROPERTY_LOCKED );
            aWindowState[7].Value = uno::makeAny( rElementData.m_aDockedData.m_bLocked );

            ::rtl::OUString aName = rElementData.m_aName;
            if ( xPersistentWindowState->hasByName( aName ))
            {
                uno::Reference< container::XNameReplace > xReplace( xPersistentWindowState, uno::UNO_QUERY );
                xReplace->replaceByName( aName, uno::makeAny( aWindowState ));
            }
            else
            {
                uno::Reference< container::XNameContainer > xInsert( xPersistentWindowState, uno::UNO_QUERY );
                xInsert->insertByName( aName, uno::makeAny( aWindowState ));
            }
        }
        catch ( uno::Exception& )
        {
        }
    }

    // Reset flag
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    aWriteLock.lock();
    m_bStoreWindowState = false;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
}

void ToolbarLayoutManager::implts_writeNewWindowStateData( const rtl::OUString aName, const uno::Reference< awt::XWindow >& xWindow )
{
    awt::Rectangle aPos;
    awt::Size      aSize;
    bool           bVisible( false );
    bool           bFloating( true );

    if ( xWindow.is() )
    {
        uno::Reference< awt::XDockableWindow > xDockWindow( xWindow, uno::UNO_QUERY );
        if ( xDockWindow.is() )
            bFloating = xDockWindow->isFloating();

        uno::Reference< awt::XWindow2 > xWindow2( xWindow, uno::UNO_QUERY );
        if( xWindow2.is() )
        {
            aPos     = xWindow2->getPosSize();
            aSize    = xWindow2->getOutputSize();   // always use output size for consistency
            bVisible = xWindow2->isVisible();
        }

        /* SAFE AREA ----------------------------------------------------------------------------------------------- */
        WriteGuard aWriteLock( m_aLock );
        UIElement& rUIElement = impl_findToolbar( aName );
        if ( rUIElement.m_xUIElement.is() )
        {
            rUIElement.m_bVisible   = bVisible;
            rUIElement.m_bFloating  = bFloating;
            if ( bFloating )
            {
                rUIElement.m_aFloatingData.m_aPos  = ::Point( aPos.X, aPos.Y );
                rUIElement.m_aFloatingData.m_aSize = ::Size( aSize.Width, aSize.Height );
            }
        }

        implts_writeWindowStateData( rUIElement );

        aWriteLock.unlock();
        /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    }
}

/******************************************************************************
                        LOOKUP PART FOR TOOLBARS
******************************************************************************/

UIElement& ToolbarLayoutManager::impl_findToolbar( const rtl::OUString& aName )
{
    static UIElement aEmptyElement;
    UIElementVector::iterator pIter;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    for ( pIter = m_aUIElements.begin(); pIter != m_aUIElements.end(); pIter++ )
    {
        if ( pIter->m_aName == aName )
            return *pIter;
    }

    return aEmptyElement;
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
}

UIElement ToolbarLayoutManager::implts_findToolbar( const rtl::OUString& aName )
{
    UIElement aEmptyElement;
    UIElementVector::iterator pIter;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    for ( pIter = m_aUIElements.begin(); pIter != m_aUIElements.end(); pIter++ )
    {
        if ( pIter->m_aName == aName )
            return *pIter;
    }

    return aEmptyElement;
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
}

UIElement ToolbarLayoutManager::implts_findToolbar(
    const uno::Reference< uno::XInterface >& xToolbar )
{
    UIElement                       aToolbar;
    UIElementVector::const_iterator pIter;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    for ( pIter = m_aUIElements.begin(); pIter != m_aUIElements.end(); pIter++ )
    {
        if ( pIter->m_xUIElement.is() )
        {
            uno::Reference< uno::XInterface > xIfac( pIter->m_xUIElement->getRealInterface(), uno::UNO_QUERY );
            if ( xIfac == xToolbar )
            {
                aToolbar = *pIter;
                break;
            }
        }
    }
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    return aToolbar;
}

bool ToolbarLayoutManager::implts_insertToolbar( const UIElement& rUIElement )
{
    UIElement aTempData;
    bool      bFound( false );
    bool      bResult( false );

    aTempData = implts_findToolbar( rUIElement.m_aName );
    if ( aTempData.m_aName == rUIElement.m_aName )
        bFound = true;

    if ( !bFound )
    {
        WriteGuard aWriteLock( m_aLock );
        m_aUIElements.push_back( rUIElement );
        bResult = true;
    }

    return bResult;
}

void ToolbarLayoutManager::implts_setToolbar( const UIElement& rUIElement )
{
    WriteGuard aWriteLock( m_aLock );
    UIElement& rData = impl_findToolbar( rUIElement.m_aName );
    if ( rData.m_aName == rUIElement.m_aName )
        rData = rUIElement;
    else
        m_aUIElements.push_back( rUIElement );
}

/******************************************************************************
                        LAYOUT CODE PART FOR TOOLBARS
******************************************************************************/

::Point ToolbarLayoutManager::implts_findNextCascadeFloatingPos()
{
    const sal_Int32 nHotZoneX       = 50;
    const sal_Int32 nHotZoneY       = 50;
    const sal_Int32 nCascadeIndentX = 15;
    const sal_Int32 nCascadeIndentY = 15;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    uno::Reference< awt::XWindow2 > xContainerWindow  = m_xContainerWindow;
    uno::Reference< awt::XWindow > xTopDockingWindow  = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_TOP];
    uno::Reference< awt::XWindow > xLeftDockingWindow = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_LEFT];
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    ::Point aStartPos( nCascadeIndentX, nCascadeIndentY );
    ::Point aCurrPos( aStartPos );
    css::awt::Rectangle aRect;

    Window* pContainerWindow( 0 );
    if ( xContainerWindow.is() )
    {
        vos::OGuard aGuard( Application::GetSolarMutex() );
        pContainerWindow = VCLUnoHelper::GetWindow( xContainerWindow );
        if ( pContainerWindow )
            aStartPos = pContainerWindow->OutputToScreenPixel( aStartPos );
    }

    // Determine size of top and left docking area
    css::awt::Rectangle aTopRect  = xTopDockingWindow->getPosSize();
    css::awt::Rectangle aLeftRect = xLeftDockingWindow->getPosSize();

    aStartPos.X() += aLeftRect.Width + nCascadeIndentX;
    aStartPos.Y() += aTopRect.Height + nCascadeIndentY;
    aCurrPos = aStartPos;

    // Try to find a cascaded position for the new floating window
    UIElementVector::const_iterator pIter;
    for ( pIter = m_aUIElements.begin(); pIter != m_aUIElements.end(); pIter++ )
    {
        if ( pIter->m_xUIElement.is() )
        {
            uno::Reference< awt::XDockableWindow > xDockWindow( pIter->m_xUIElement->getRealInterface(), uno::UNO_QUERY );
            uno::Reference< awt::XWindow > xWindow( xDockWindow, uno::UNO_QUERY );
            if ( xDockWindow.is() && xDockWindow->isFloating() )
            {
                vos::OGuard aGuard( Application::GetSolarMutex() );
                Window* pWindow = VCLUnoHelper::GetWindow( xWindow );
                if ( pWindow && pWindow->IsVisible() )
                {
                    css::awt::Rectangle aFloatRect = xWindow->getPosSize();
                    if ((( aFloatRect.X - nHotZoneX ) <= aCurrPos.X() ) &&
                        ( aFloatRect.X >= aCurrPos.X() ) &&
                        (( aFloatRect.Y - nHotZoneY ) <= aCurrPos.Y() ) &&
                        ( aFloatRect.Y >= aCurrPos.Y() ))
                    {
                        aCurrPos.X() = aFloatRect.X + nCascadeIndentX;
                        aCurrPos.Y() = aFloatRect.Y + nCascadeIndentY;
                    }
                }
            }
        }
    }

    return aCurrPos;
}

void ToolbarLayoutManager::implts_sortUIElements()
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    UIElementVector::iterator pIterStart = m_aUIElements.begin();
    UIElementVector::iterator pIterEnd   = m_aUIElements.end();

    std::stable_sort( pIterStart, pIterEnd ); // first created element should first

    // We have to reset our temporary flags.
    UIElementVector::iterator pIter;
    for ( pIter = m_aUIElements.begin(); pIter != m_aUIElements.end(); pIter++ )
        pIter->m_bUserActive = sal_False;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
}

::Size ToolbarLayoutManager::implts_getTopBottomDockingAreaSizes()
{
    ::Size                         aSize;
    uno::Reference< awt::XWindow > xTopDockingAreaWindow;
    uno::Reference< awt::XWindow > xBottomDockingAreaWindow;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    xTopDockingAreaWindow       = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_TOP];
    xBottomDockingAreaWindow    = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_BOTTOM];
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    if ( xTopDockingAreaWindow.is() )
        aSize.Width() = xTopDockingAreaWindow->getPosSize().Height;
    if ( xBottomDockingAreaWindow.is() )
        aSize.Height() = xBottomDockingAreaWindow->getPosSize().Height;

    return aSize;
}

void ToolbarLayoutManager::implts_getDockingAreaElementInfos( ui::DockingArea eDockingArea, std::vector< SingleRowColumnWindowData >& rRowColumnsWindowData )
{
    std::vector< UIElement > aWindowVector;

    if (( eDockingArea < ui::DockingArea_DOCKINGAREA_TOP ) ||
        ( eDockingArea > ui::DockingArea_DOCKINGAREA_RIGHT ))
        eDockingArea = ui::DockingArea_DOCKINGAREA_TOP;

    uno::Reference< awt::XWindow > xDockAreaWindow;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    aWindowVector.reserve(m_aUIElements.size());
    xDockAreaWindow = m_xDockAreaWindows[eDockingArea];
    UIElementVector::iterator   pIter;
    for ( pIter = m_aUIElements.begin(); pIter != m_aUIElements.end(); pIter++ )
    {
        if ( pIter->m_aDockedData.m_nDockedArea == eDockingArea && pIter->m_bVisible && !pIter->m_bFloating )
        {
            uno::Reference< ui::XUIElement > xUIElement( pIter->m_xUIElement );
            if ( xUIElement.is() )
            {
                uno::Reference< css::awt::XWindow > xWindow( xUIElement->getRealInterface(), uno::UNO_QUERY );
                uno::Reference< css::awt::XDockableWindow > xDockWindow( xWindow, uno::UNO_QUERY );
                if ( xDockWindow.is() )
                {
                    // docked windows
                    aWindowVector.push_back( *pIter );
                }
            }
        }
    }
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    rRowColumnsWindowData.clear();

    // Collect data from windows that are on the same row/column
    sal_Int32 j;
    sal_Int32 nIndex( 0 );
    sal_Int32 nLastPos( 0 );
    sal_Int32 nCurrPos( -1 );
    sal_Int32 nLastRowColPixelPos( 0 );
    awt::Rectangle aDockAreaRect;

    if ( xDockAreaWindow.is() )
        aDockAreaRect = xDockAreaWindow->getPosSize();

    if ( eDockingArea == ui::DockingArea_DOCKINGAREA_TOP )
        nLastRowColPixelPos = 0;
    else if ( eDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM )
        nLastRowColPixelPos = aDockAreaRect.Height;
    else if ( eDockingArea == ui::DockingArea_DOCKINGAREA_LEFT )
        nLastRowColPixelPos = 0;
    else
        nLastRowColPixelPos = aDockAreaRect.Width;

    const sal_uInt32 nCount = aWindowVector.size();
    for ( j = 0; j < sal_Int32( nCount); j++ )
    {
        const UIElement& rElement = aWindowVector[j];
        uno::Reference< awt::XWindow > xWindow;
        uno::Reference< ui::XUIElement > xUIElement( rElement.m_xUIElement );
        awt::Rectangle aPosSize;

        if ( !lcl_checkUIElement(xUIElement,aPosSize,xWindow) )
            continue;
        if (( eDockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
            ( eDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ))
        {
            if ( nCurrPos == -1 )
            {
                nCurrPos = rElement.m_aDockedData.m_aPos.Y();
                nLastPos = 0;

                SingleRowColumnWindowData aRowColumnWindowData;
                aRowColumnWindowData.nRowColumn = nCurrPos;
                rRowColumnsWindowData.push_back( aRowColumnWindowData );
            }

            sal_Int32 nSpace( 0 );
            if ( rElement.m_aDockedData.m_aPos.Y() != nCurrPos )
            {
                if ( eDockingArea == ui::DockingArea_DOCKINGAREA_TOP )
                    nLastRowColPixelPos += rRowColumnsWindowData[nIndex].nStaticSize;
                else
                    nLastRowColPixelPos -= rRowColumnsWindowData[nIndex].nStaticSize;
                ++nIndex;
                nLastPos = 0;
                nCurrPos = rElement.m_aDockedData.m_aPos.Y();
                SingleRowColumnWindowData aRowColumnWindowData;
                aRowColumnWindowData.nRowColumn = nCurrPos;
                rRowColumnsWindowData.push_back( aRowColumnWindowData );
            }

            // Calc space before an element and store it
            nSpace = ( rElement.m_aDockedData.m_aPos.X() - nLastPos );
            if ( rElement.m_aDockedData.m_aPos.X() >= nLastPos )
            {
                rRowColumnsWindowData[nIndex].nSpace += nSpace;
                nLastPos = rElement.m_aDockedData.m_aPos.X() + aPosSize.Width;
            }
            else
            {
                nSpace = 0;
                nLastPos += aPosSize.Width;
            }
            rRowColumnsWindowData[nIndex].aRowColumnSpace.push_back( nSpace );

            rRowColumnsWindowData[nIndex].aRowColumnWindows.push_back( xWindow );
            rRowColumnsWindowData[nIndex].aUIElementNames.push_back( rElement.m_aName );
            rRowColumnsWindowData[nIndex].aRowColumnWindowSizes.push_back(
                css::awt::Rectangle( rElement.m_aDockedData.m_aPos.X(),
                                     rElement.m_aDockedData.m_aPos.Y(),
                                     aPosSize.Width,
                                     aPosSize.Height ));
            if ( rRowColumnsWindowData[nIndex].nStaticSize < aPosSize.Height )
                rRowColumnsWindowData[nIndex].nStaticSize = aPosSize.Height;
            if ( eDockingArea == ui::DockingArea_DOCKINGAREA_TOP )
                rRowColumnsWindowData[nIndex].aRowColumnRect = css::awt::Rectangle( 0, nLastRowColPixelPos,
                                                                                    aDockAreaRect.Width, aPosSize.Height );
            else
                rRowColumnsWindowData[nIndex].aRowColumnRect = css::awt::Rectangle( 0, ( nLastRowColPixelPos - aPosSize.Height ),
                                                                                    aDockAreaRect.Width, aPosSize.Height );
            rRowColumnsWindowData[nIndex].nVarSize += aPosSize.Width + nSpace;
        }
        else
        {
            if ( nCurrPos == -1 )
            {
                nCurrPos = rElement.m_aDockedData.m_aPos.X();
                nLastPos = 0;

                SingleRowColumnWindowData aRowColumnWindowData;
                aRowColumnWindowData.nRowColumn = nCurrPos;
                rRowColumnsWindowData.push_back( aRowColumnWindowData );
            }

            sal_Int32 nSpace( 0 );
            if ( rElement.m_aDockedData.m_aPos.X() != nCurrPos )
            {
                if ( eDockingArea == ui::DockingArea_DOCKINGAREA_LEFT )
                    nLastRowColPixelPos += rRowColumnsWindowData[nIndex].nStaticSize;
                else
                    nLastRowColPixelPos -= rRowColumnsWindowData[nIndex].nStaticSize;
                ++nIndex;
                nLastPos = 0;
                nCurrPos = rElement.m_aDockedData.m_aPos.X();
                SingleRowColumnWindowData aRowColumnWindowData;
                aRowColumnWindowData.nRowColumn = nCurrPos;
                rRowColumnsWindowData.push_back( aRowColumnWindowData );
            }

            // Calc space before an element and store it
            nSpace = ( rElement.m_aDockedData.m_aPos.Y() - nLastPos );
            if ( rElement.m_aDockedData.m_aPos.Y() > nLastPos )
            {
                rRowColumnsWindowData[nIndex].nSpace += nSpace;
                nLastPos = rElement.m_aDockedData.m_aPos.Y() + aPosSize.Height;
            }
            else
            {
                nSpace = 0;
                nLastPos += aPosSize.Height;
            }
            rRowColumnsWindowData[nIndex].aRowColumnSpace.push_back( nSpace );

            rRowColumnsWindowData[nIndex].aRowColumnWindows.push_back( xWindow );
            rRowColumnsWindowData[nIndex].aUIElementNames.push_back( rElement.m_aName );
            rRowColumnsWindowData[nIndex].aRowColumnWindowSizes.push_back(
                css::awt::Rectangle( rElement.m_aDockedData.m_aPos.X(),
                                     rElement.m_aDockedData.m_aPos.Y(),
                                     aPosSize.Width,
                                     aPosSize.Height ));
            if ( rRowColumnsWindowData[nIndex].nStaticSize < aPosSize.Width )
                rRowColumnsWindowData[nIndex].nStaticSize = aPosSize.Width;
            if ( eDockingArea == ui::DockingArea_DOCKINGAREA_LEFT )
                rRowColumnsWindowData[nIndex].aRowColumnRect = css::awt::Rectangle( nLastRowColPixelPos, 0,
                                                                                    aPosSize.Width, aDockAreaRect.Height );
            else
                rRowColumnsWindowData[nIndex].aRowColumnRect = css::awt::Rectangle( ( nLastRowColPixelPos - aPosSize.Width ), 0,
                                                                                    aPosSize.Width, aDockAreaRect.Height );
            rRowColumnsWindowData[nIndex].nVarSize += aPosSize.Height + nSpace;
        }
    }
}

void ToolbarLayoutManager::implts_getDockingAreaElementInfoOnSingleRowCol( ui::DockingArea eDockingArea, sal_Int32 nRowCol, SingleRowColumnWindowData& rRowColumnWindowData )
{
    std::vector< UIElement > aWindowVector;

    if (( eDockingArea < ui::DockingArea_DOCKINGAREA_TOP ) ||
        ( eDockingArea > ui::DockingArea_DOCKINGAREA_RIGHT ))
        eDockingArea = ui::DockingArea_DOCKINGAREA_TOP;

    sal_Bool bHorzDockArea = (( eDockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
                              ( eDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ));

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    UIElementVector::iterator   pIter;
    UIElementVector::iterator   pEnd = m_aUIElements.end();
    for ( pIter = m_aUIElements.begin(); pIter != pEnd; pIter++ )
    {
        if ( pIter->m_aDockedData.m_nDockedArea == eDockingArea )
        {
            sal_Bool                bSameRowCol = bHorzDockArea ?
                                                   ( pIter->m_aDockedData.m_aPos.Y() == nRowCol ) :
                                                   ( pIter->m_aDockedData.m_aPos.X() == nRowCol );
            uno::Reference< ui::XUIElement > xUIElement( pIter->m_xUIElement );

            if ( bSameRowCol && xUIElement.is() )
            {
                uno::Reference< awt::XWindow > xWindow( xUIElement->getRealInterface(), uno::UNO_QUERY );
                if ( xWindow.is() )
                {
                    vos::OGuard aGuard( Application::GetSolarMutex() );
                    Window* pWindow = VCLUnoHelper::GetWindow( xWindow );
                    uno::Reference< awt::XDockableWindow > xDockWindow( xWindow, uno::UNO_QUERY );
                    if ( pWindow && pIter->m_bVisible && xDockWindow.is() && !pIter->m_bFloating )
                    {
                        // docked windows
                        aWindowVector.push_back( *pIter );
                    }
                }
            }
        }
    }
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    // Initialize structure
    rRowColumnWindowData.aUIElementNames.clear();
    rRowColumnWindowData.aRowColumnWindows.clear();
    rRowColumnWindowData.aRowColumnWindowSizes.clear();
    rRowColumnWindowData.aRowColumnSpace.clear();
    rRowColumnWindowData.nVarSize = 0;
    rRowColumnWindowData.nStaticSize = 0;
    rRowColumnWindowData.nSpace = 0;
    rRowColumnWindowData.nRowColumn = nRowCol;

    // Collect data from windows that are on the same row/column
    sal_Int32 j;
    sal_Int32 nLastPos( 0 );

    const sal_uInt32 nCount = aWindowVector.size();
    for ( j = 0; j < sal_Int32( nCount); j++ )
    {
        const UIElement& rElement = aWindowVector[j];
        uno::Reference< awt::XWindow > xWindow;
        uno::Reference< ui::XUIElement > xUIElement( rElement.m_xUIElement );
        awt::Rectangle aPosSize;
        if ( !lcl_checkUIElement(xUIElement,aPosSize,xWindow) )
            continue;

        sal_Int32 nSpace;
        if (( eDockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
            ( eDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ))
        {
            nSpace = ( rElement.m_aDockedData.m_aPos.X() - nLastPos );

            // Calc space before an element and store it
            if ( rElement.m_aDockedData.m_aPos.X() > nLastPos )
                rRowColumnWindowData.nSpace += nSpace;
            else
                nSpace = 0;

            nLastPos = rElement.m_aDockedData.m_aPos.X() + aPosSize.Width;


            rRowColumnWindowData.aRowColumnWindowSizes.push_back(
                css::awt::Rectangle( rElement.m_aDockedData.m_aPos.X(),
                                     rElement.m_aDockedData.m_aPos.Y(),
                                     aPosSize.Width,
                                     aPosSize.Height ));
            if ( rRowColumnWindowData.nStaticSize < aPosSize.Height )
                rRowColumnWindowData.nStaticSize = aPosSize.Height;
            rRowColumnWindowData.nVarSize += aPosSize.Width;
        }
        else
        {
            // Calc space before an element and store it
            nSpace = ( rElement.m_aDockedData.m_aPos.Y() - nLastPos );
            if ( rElement.m_aDockedData.m_aPos.Y() > nLastPos )
                rRowColumnWindowData.nSpace += nSpace;
            else
                nSpace = 0;

            nLastPos = rElement.m_aDockedData.m_aPos.Y() + aPosSize.Height;

            rRowColumnWindowData.aRowColumnWindowSizes.push_back(
                css::awt::Rectangle( rElement.m_aDockedData.m_aPos.X(),
                                     rElement.m_aDockedData.m_aPos.Y(),
                                     aPosSize.Width,
                                     aPosSize.Height ));
            if ( rRowColumnWindowData.nStaticSize < aPosSize.Width )
                rRowColumnWindowData.nStaticSize = aPosSize.Width;
            rRowColumnWindowData.nVarSize += aPosSize.Height;
        }

        rRowColumnWindowData.aUIElementNames.push_back( rElement.m_aName );
        rRowColumnWindowData.aRowColumnWindows.push_back( xWindow );
        rRowColumnWindowData.aRowColumnSpace.push_back( nSpace );
        rRowColumnWindowData.nVarSize += nSpace;
    }
}

::Rectangle ToolbarLayoutManager::implts_getWindowRectFromRowColumn(
    ui::DockingArea DockingArea,
    const SingleRowColumnWindowData& rRowColumnWindowData,
    const ::Point& rMousePos,
    const rtl::OUString& rExcludeElementName )
{
    ::Rectangle aWinRect;

    if (( DockingArea < ui::DockingArea_DOCKINGAREA_TOP ) ||
        ( DockingArea > ui::DockingArea_DOCKINGAREA_RIGHT ))
        DockingArea = ui::DockingArea_DOCKINGAREA_TOP;

    if ( rRowColumnWindowData.aRowColumnWindows.empty() )
        return aWinRect;
    else
    {
        /* SAFE AREA ----------------------------------------------------------------------------------------------- */
        ReadGuard aReadLock( m_aLock );
        uno::Reference< awt::XWindow2 > xContainerWindow  = m_xContainerWindow;
        uno::Reference< awt::XWindow > xDockingAreaWindow = m_xDockAreaWindows[DockingArea];
        aReadLock.unlock();
        /* SAFE AREA ----------------------------------------------------------------------------------------------- */

        // Calc correct position of the column/row rectangle to be able to compare it with mouse pos/tracking rect
        vos::OGuard aGuard( Application::GetSolarMutex() );

        // Retrieve output size from container Window
        Window* pContainerWindow( VCLUnoHelper::GetWindow( xContainerWindow ));
        Window* pDockingAreaWindow( VCLUnoHelper::GetWindow( xDockingAreaWindow ));
        if ( pDockingAreaWindow && pContainerWindow )
        {
            const sal_uInt32 nCount = rRowColumnWindowData.aRowColumnWindows.size();
            for ( sal_uInt32 i = 0; i < nCount; i++ )
            {
                awt::Rectangle aWindowRect = rRowColumnWindowData.aRowColumnWindows[i]->getPosSize();
                ::Rectangle aRect( aWindowRect.X, aWindowRect.Y, aWindowRect.X+aWindowRect.Width, aWindowRect.Y+aWindowRect.Height );
                aRect.SetPos( pContainerWindow->ScreenToOutputPixel( pDockingAreaWindow->OutputToScreenPixel( aRect.TopLeft() )));
                if ( aRect.IsInside( rMousePos ))
                {
                    // Check if we have found the excluded element. If yes, we have to provide an empty rectangle.
                    // We prevent that a toolbar cannot be moved when the mouse pointer is inside its own rectangle!
                    if ( rExcludeElementName != rRowColumnWindowData.aUIElementNames[i] )
                        return aRect;
                    else
                        break;
                }
            }
        }
    }

    return aWinRect;
}

::Rectangle ToolbarLayoutManager::implts_determineFrontDockingRect(
    ui::DockingArea        eDockingArea,
    sal_Int32              nRowCol,
    const ::Rectangle&     rDockedElementRect,
    const ::rtl::OUString& rMovedElementName,
    const ::Rectangle&     rMovedElementRect )
{
    SingleRowColumnWindowData aRowColumnWindowData;

    sal_Bool bHorzDockArea = (( eDockingArea == ui::DockingArea_DOCKINGAREA_TOP    ) ||
                              ( eDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ));

    implts_getDockingAreaElementInfoOnSingleRowCol( eDockingArea, nRowCol, aRowColumnWindowData );
    if ( aRowColumnWindowData.aRowColumnWindows.empty() )
        return rMovedElementRect;
    else
    {
        sal_Int32 nSpace( 0 );
        ::Rectangle aFrontDockingRect( rMovedElementRect );
        const sal_uInt32 nCount = aRowColumnWindowData.aRowColumnWindows.size();
        for ( sal_uInt32 i = 0; i < nCount; i++ )
        {
            if ( bHorzDockArea )
            {
                if ( aRowColumnWindowData.aRowColumnWindowSizes[i].X >= rDockedElementRect.Left() )
                {
                    nSpace += aRowColumnWindowData.aRowColumnSpace[i];
                    break;
                }
                else if ( aRowColumnWindowData.aUIElementNames[i] == rMovedElementName )
                    nSpace += aRowColumnWindowData.aRowColumnWindowSizes[i].Width +
                              aRowColumnWindowData.aRowColumnSpace[i];
                else
                    nSpace = 0;
            }
            else
            {
                if ( aRowColumnWindowData.aRowColumnWindowSizes[i].Y >= rDockedElementRect.Top() )
                {
                    nSpace += aRowColumnWindowData.aRowColumnSpace[i];
                    break;
                }
                else if ( aRowColumnWindowData.aUIElementNames[i] == rMovedElementName )
                    nSpace += aRowColumnWindowData.aRowColumnWindowSizes[i].Height +
                              aRowColumnWindowData.aRowColumnSpace[i];
                else
                    nSpace = 0;
            }
        }

        if ( nSpace > 0 )
        {
            sal_Int32 nMove = std::min( nSpace, static_cast<sal_Int32>(aFrontDockingRect.getWidth()) );
            if ( bHorzDockArea )
                aFrontDockingRect.Move( -nMove, 0 );
            else
                aFrontDockingRect.Move( 0, -nMove );
        }

        return aFrontDockingRect;
    }
}

void ToolbarLayoutManager::implts_findNextDockingPos( ui::DockingArea DockingArea, const ::Size& aUIElementSize, ::Point& rVirtualPos, ::Point& rPixelPos )
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    uno::Reference< awt::XWindow > xDockingWindow = m_xDockAreaWindows[DockingArea];
    ::Size                         aDockingWinSize;
    Window*                        pDockingWindow( 0 );
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    if (( DockingArea < ui::DockingArea_DOCKINGAREA_TOP ) ||
        ( DockingArea > ui::DockingArea_DOCKINGAREA_RIGHT ))
        DockingArea = ui::DockingArea_DOCKINGAREA_TOP;

    {
        // Retrieve output size from container Window
        vos::OGuard aGuard( Application::GetSolarMutex() );
        pDockingWindow  = VCLUnoHelper::GetWindow( xDockingWindow );
        if ( pDockingWindow )
            aDockingWinSize = pDockingWindow->GetOutputSizePixel();
    }

    sal_Int32 nFreeRowColPixelPos( 0 );
    sal_Int32 nMaxSpace( 0 );
    sal_Int32 nNeededSpace( 0 );
    sal_Int32 nTopDockingAreaSize( 0 );

    if (( DockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
        ( DockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ))
    {
        nMaxSpace    = aDockingWinSize.Width();
        nNeededSpace = aUIElementSize.Width();
    }
    else
    {
        nMaxSpace              = aDockingWinSize.Height();
        nNeededSpace           = aUIElementSize.Height();
        nTopDockingAreaSize    = implts_getTopBottomDockingAreaSizes().Width();
    }

    std::vector< SingleRowColumnWindowData > aRowColumnsWindowData;

    implts_getDockingAreaElementInfos( DockingArea, aRowColumnsWindowData );
    sal_Int32 nPixelPos( 0 );
    const sal_uInt32 nCount = aRowColumnsWindowData.size();
    for ( sal_uInt32 i = 0; i < nCount; i++ )
    {
        SingleRowColumnWindowData& rRowColumnWindowData = aRowColumnsWindowData[i];

        if (( DockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ) ||
            ( DockingArea == ui::DockingArea_DOCKINGAREA_RIGHT  ))
            nPixelPos += rRowColumnWindowData.nStaticSize;

        if ((( nMaxSpace - rRowColumnWindowData.nVarSize ) >= nNeededSpace ) ||
            ( rRowColumnWindowData.nSpace >= nNeededSpace ))
        {
            // Check current row where we can find the needed space
            sal_Int32 nCurrPos( 0 );
            const sal_uInt32 nWindowSizesCount = rRowColumnWindowData.aRowColumnWindowSizes.size();
            for ( sal_uInt32 j = 0; j < nWindowSizesCount; j++ )
            {
                css::awt::Rectangle rRect   = rRowColumnWindowData.aRowColumnWindowSizes[j];
                sal_Int32&          rSpace  = rRowColumnWindowData.aRowColumnSpace[j];
                if (( DockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
                    ( DockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ))
                {
                    if ( rSpace >= nNeededSpace )
                    {
                        rVirtualPos = ::Point( nCurrPos, rRowColumnWindowData.nRowColumn );
                        if ( DockingArea == ui::DockingArea_DOCKINGAREA_TOP )
                            rPixelPos   = ::Point( nCurrPos, nPixelPos );
                        else
                            rPixelPos   = ::Point( nCurrPos, aDockingWinSize.Height() - nPixelPos );
                        return;
                    }
                    nCurrPos = rRect.X + rRect.Width;
                }
                else
                {
                    if ( rSpace >= nNeededSpace )
                    {
                        rVirtualPos = ::Point( rRowColumnWindowData.nRowColumn, nCurrPos );
                        if ( DockingArea == ui::DockingArea_DOCKINGAREA_LEFT )
                            rPixelPos   = ::Point( nPixelPos, nTopDockingAreaSize + nCurrPos );
                        else
                            rPixelPos   = ::Point( aDockingWinSize.Width() - nPixelPos , nTopDockingAreaSize + nCurrPos );
                        return;
                    }
                    nCurrPos = rRect.Y + rRect.Height;
                }
            }

            if (( nCurrPos + nNeededSpace ) <= nMaxSpace )
            {
                if (( DockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
                    ( DockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ))
                {
                    rVirtualPos = ::Point( nCurrPos, rRowColumnWindowData.nRowColumn );
                    if ( DockingArea == ui::DockingArea_DOCKINGAREA_TOP )
                        rPixelPos   = ::Point( nCurrPos, nPixelPos );
                    else
                        rPixelPos   = ::Point( nCurrPos, aDockingWinSize.Height() - nPixelPos );
                    return;
                }
                else
                {
                    rVirtualPos = ::Point( rRowColumnWindowData.nRowColumn, nCurrPos );
                    if ( DockingArea == ui::DockingArea_DOCKINGAREA_LEFT )
                        rPixelPos   = ::Point( nPixelPos, nTopDockingAreaSize + nCurrPos );
                    else
                        rPixelPos   = ::Point( aDockingWinSize.Width() - nPixelPos , nTopDockingAreaSize + nCurrPos );
                    return;
                }
            }
        }

        if (( DockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
            ( DockingArea == ui::DockingArea_DOCKINGAREA_LEFT  ))
            nPixelPos += rRowColumnWindowData.nStaticSize;
    }

    sal_Int32 nNextFreeRowCol( 0 );
    sal_Int32 nRowColumnsCount = aRowColumnsWindowData.size();
    if ( nRowColumnsCount > 0 )
        nNextFreeRowCol = aRowColumnsWindowData[nRowColumnsCount-1].nRowColumn+1;
    else
        nNextFreeRowCol = 0;

    if ( nNextFreeRowCol == 0 )
    {
        if ( DockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM )
            nFreeRowColPixelPos = aDockingWinSize.Height() - aUIElementSize.Height();
        else if ( DockingArea == ui::DockingArea_DOCKINGAREA_RIGHT  )
            nFreeRowColPixelPos = aDockingWinSize.Width() - aUIElementSize.Width();
    }

    if (( DockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
        ( DockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ))
    {
        rVirtualPos = ::Point( 0, nNextFreeRowCol );
        if ( DockingArea == ui::DockingArea_DOCKINGAREA_TOP )
            rPixelPos = ::Point( 0, nFreeRowColPixelPos );
        else
            rPixelPos = ::Point( 0, aDockingWinSize.Height() - nFreeRowColPixelPos );
    }
    else
    {
        rVirtualPos = ::Point( nNextFreeRowCol, 0 );
        rPixelPos   = ::Point( aDockingWinSize.Width() - nFreeRowColPixelPos, 0 );
    }
}

void ToolbarLayoutManager::implts_calcWindowPosSizeOnSingleRowColumn(
    sal_Int32 nDockingArea,
    sal_Int32 nOffset,
    SingleRowColumnWindowData& rRowColumnWindowData,
    const ::Size& rContainerSize )
{
    sal_Int32   nDiff( 0 );
    sal_Int32   nRCSpace( rRowColumnWindowData.nSpace );
    sal_Int32   nTopDockingAreaSize;
    sal_Int32   nBottomDockingAreaSize;
    sal_Int32   nContainerClientSize;

    if ( rRowColumnWindowData.aRowColumnWindows.empty() )
        return;

    if (( nDockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
        ( nDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ))
    {
        nContainerClientSize = rContainerSize.Width();
        nDiff = nContainerClientSize - rRowColumnWindowData.nVarSize;
    }
    else
    {
        nTopDockingAreaSize    = implts_getTopBottomDockingAreaSizes().Width();
        nBottomDockingAreaSize = implts_getTopBottomDockingAreaSizes().Height();
        nContainerClientSize   = ( rContainerSize.Height() - nTopDockingAreaSize - nBottomDockingAreaSize );
        nDiff = nContainerClientSize - rRowColumnWindowData.nVarSize;
    }

    const sal_uInt32 nCount = rRowColumnWindowData.aRowColumnWindowSizes.size();
    if (( nDiff < 0 ) && ( nRCSpace > 0 ))
    {
        // First we try to reduce the size of blank space before/behind docked windows
        sal_Int32 i = nCount - 1;
        while ( i >= 0 )
        {
            sal_Int32 nSpace = rRowColumnWindowData.aRowColumnSpace[i];
            if ( nSpace >= -nDiff )
            {

                if (( nDockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
                    ( nDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ))
                {
                    // Try to move this and all user elements behind with the calculated difference
                    for ( sal_uInt32 j = i; j < nCount ; j++ )
                        rRowColumnWindowData.aRowColumnWindowSizes[j].X += nDiff;
                }
                else
                {
                    // Try to move this and all user elements behind with the calculated difference
                    for ( sal_uInt32 j = i; j < nCount ; j++ )
                        rRowColumnWindowData.aRowColumnWindowSizes[j].Y += nDiff;
                }
                nDiff = 0;

                break;
            }
            else if ( nSpace > 0 )
            {
                if (( nDockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
                    ( nDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ))
                {
                    // Try to move this and all user elements behind with the calculated difference
                    for ( sal_uInt32 j = i; j < nCount; j++ )
                        rRowColumnWindowData.aRowColumnWindowSizes[j].X -= nSpace;
                }
                else
                {
                    // Try to move this and all user elements behind with the calculated difference
                    for ( sal_uInt32 j = i; j < nCount; j++ )
                        rRowColumnWindowData.aRowColumnWindowSizes[j].Y -= nSpace;
                }
                nDiff += nSpace;
            }
            --i;
        }
    }

    // Check if we have to reduce further
    if ( nDiff < 0 )
    {
        // Now we have to reduce the size of certain docked windows
        sal_Int32 i = sal_Int32( nCount - 1 );
        while ( i >= 0 )
        {
            css::awt::Rectangle& rWinRect = rRowColumnWindowData.aRowColumnWindowSizes[i];
            ::Size               aMinSize;

            vos::OGuard aGuard( Application::GetSolarMutex() );
            {
                uno::Reference< awt::XWindow > xWindow = rRowColumnWindowData.aRowColumnWindows[i];
                Window* pWindow = VCLUnoHelper::GetWindow( xWindow );
                if ( pWindow && pWindow->GetType() == WINDOW_TOOLBOX )
                    aMinSize = ((ToolBox *)pWindow)->CalcMinimumWindowSizePixel();
            }

            if (( aMinSize.Width() > 0 ) && ( aMinSize.Height() > 0 ))
            {
                if (( nDockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
                    ( nDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ))
                {
                    sal_Int32 nMaxReducation = ( rWinRect.Width - aMinSize.Width() );
                    if ( nMaxReducation >= -nDiff )
                    {
                        rWinRect.Width = rWinRect.Width + nDiff;
                        nDiff = 0;
                    }
                    else
                    {
                        rWinRect.Width = aMinSize.Width();
                        nDiff += nMaxReducation;
                    }

                    // Try to move this and all user elements behind with the calculated difference
                    for ( sal_uInt32 j = i; j < nCount; j++ )
                        rRowColumnWindowData.aRowColumnWindowSizes[j].X += nDiff;
                }
                else
                {
                    sal_Int32 nMaxReducation = ( rWinRect.Height - aMinSize.Height() );
                    if ( nMaxReducation >= -nDiff )
                    {
                        rWinRect.Height = rWinRect.Height + nDiff;
                        nDiff = 0;
                    }
                    else
                    {
                        rWinRect.Height = aMinSize.Height();
                        nDiff += nMaxReducation;
                    }

                    // Try to move this and all user elements behind with the calculated difference
                    for ( sal_uInt32 j = i; j < nCount; j++ )
                        rRowColumnWindowData.aRowColumnWindowSizes[j].Y += nDiff;
                }
            }

            if ( nDiff >= 0 )
                break;

            --i;
        }
    }

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    Window* pDockAreaWindow = VCLUnoHelper::GetWindow( m_xDockAreaWindows[nDockingArea] );
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    sal_Int32 nCurrPos( 0 );
    sal_Int32 nStartOffset( 0 );

    vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( nDockingArea == ui::DockingArea_DOCKINGAREA_RIGHT )
        nStartOffset = pDockAreaWindow->GetSizePixel().Width() - rRowColumnWindowData.nStaticSize;
    else if ( nDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM )
        nStartOffset = pDockAreaWindow->GetSizePixel().Height() - rRowColumnWindowData.nStaticSize;

    for ( sal_uInt32 i = 0; i < nCount; i++ )
    {
        uno::Reference< awt::XWindow > xWindow = rRowColumnWindowData.aRowColumnWindows[i];
        Window* pWindow = VCLUnoHelper::GetWindow( xWindow );
        Window* pOldParentWindow = pWindow->GetParent();

        if ( pDockAreaWindow != pOldParentWindow )
            pWindow->SetParent( pDockAreaWindow );

        awt::Rectangle aWinRect = rRowColumnWindowData.aRowColumnWindowSizes[i];
        if ( nDockingArea == ui::DockingArea_DOCKINGAREA_TOP )
        {
            if ( aWinRect.X < nCurrPos )
                aWinRect.X = nCurrPos;
            pWindow->SetPosSizePixel( ::Point( aWinRect.X, nOffset ),
                                      ::Size( aWinRect.Width, rRowColumnWindowData.nStaticSize ));
            nCurrPos += ( aWinRect.X - nCurrPos ) + aWinRect.Width;
        }
        else if ( nDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM )
        {
            if ( aWinRect.X < nCurrPos )
                aWinRect.X = nCurrPos;
            pWindow->SetPosSizePixel( ::Point( aWinRect.X, nStartOffset - nOffset ),
                                      ::Size( aWinRect.Width, rRowColumnWindowData.nStaticSize ));
            nCurrPos += ( aWinRect.X - nCurrPos ) + aWinRect.Width;
        }
        else if ( nDockingArea == ui::DockingArea_DOCKINGAREA_LEFT )
        {
            if ( aWinRect.Y < nCurrPos )
                aWinRect.Y = nCurrPos;
            pWindow->SetPosSizePixel( ::Point( nOffset, aWinRect.Y ),
                                      ::Size( rRowColumnWindowData.nStaticSize, aWinRect.Height ));
            nCurrPos += ( aWinRect.Y - nCurrPos ) + aWinRect.Height;
        }
        else if ( nDockingArea == ui::DockingArea_DOCKINGAREA_RIGHT )
        {
            if ( aWinRect.Y < nCurrPos )
                aWinRect.Y = nCurrPos;
            pWindow->SetPosSizePixel( ::Point( nStartOffset - nOffset, aWinRect.Y ),
                                      ::Size( rRowColumnWindowData.nStaticSize, aWinRect.Height ));
            nCurrPos += ( aWinRect.Y - nCurrPos ) + aWinRect.Height;
        }
    }
}

::Rectangle ToolbarLayoutManager::implts_calcHotZoneRect( const ::Rectangle& rRect, sal_Int32 nHotZoneOffset )
{
    ::Rectangle aRect( rRect );

    aRect.Left() -= nHotZoneOffset;
    aRect.Top() -= nHotZoneOffset;
    aRect.Right() += nHotZoneOffset;
    aRect.Bottom() += nHotZoneOffset;

    return aRect;
}

void ToolbarLayoutManager::implts_calcDockingPosSize(
    UIElement&          rUIElement,
    DockingOperation&   rDockingOperation,
    ::Rectangle&        rTrackingRect,
    const Point&        rMousePos )
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    uno::Reference< awt::XWindow2 > xContainerWindow( m_xContainerWindow );
    ::Size                          aContainerWinSize;
    Window*                         pContainerWindow( 0 );
    ::Rectangle                     aDockingAreaOffsets( m_aDockingAreaOffsets );
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    {
        // Retrieve output size from container Window
        vos::OGuard aGuard( Application::GetSolarMutex() );
        pContainerWindow  = VCLUnoHelper::GetWindow( xContainerWindow );
        aContainerWinSize = pContainerWindow->GetOutputSizePixel();
    }

    if ( !rUIElement.m_xUIElement.is() )
    {
        rTrackingRect = ::Rectangle();
        return;
    }

    Window*                        pDockWindow( 0 );
    Window*                        pDockingAreaWindow( 0 );
    ToolBox*                       pToolBox( 0 );
    uno::Reference< awt::XWindow > xWindow( rUIElement.m_xUIElement->getRealInterface(), uno::UNO_QUERY );
    uno::Reference< awt::XWindow > xDockingAreaWindow;
    ::Rectangle                    aTrackingRect( rTrackingRect );
    ui::DockingArea                eDockedArea( (::com::sun::star::ui::DockingArea)rUIElement.m_aDockedData.m_nDockedArea );
    sal_Int32                      nTopDockingAreaSize( implts_getTopBottomDockingAreaSizes().Width() );
    sal_Int32                      nBottomDockingAreaSize( implts_getTopBottomDockingAreaSizes().Height() );
    sal_Bool                       bHorizontalDockArea(( eDockedArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
                                                       ( eDockedArea == ui::DockingArea_DOCKINGAREA_BOTTOM ));
    sal_Int32                      nMaxLeftRightDockAreaSize = aContainerWinSize.Height() -
                                                               nTopDockingAreaSize -
                                                               nBottomDockingAreaSize -
                                                               aDockingAreaOffsets.Top() -
                                                               aDockingAreaOffsets.Bottom();
    ::Rectangle                    aDockingAreaRect;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    aReadLock.lock();
    xDockingAreaWindow = m_xDockAreaWindows[eDockedArea];
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    {
        vos::OGuard aGuard( Application::GetSolarMutex() );
        pDockingAreaWindow = VCLUnoHelper::GetWindow( xDockingAreaWindow );
        pDockWindow        = VCLUnoHelper::GetWindow( xWindow );
        if ( pDockWindow && pDockWindow->GetType() == WINDOW_TOOLBOX )
            pToolBox = (ToolBox *)pDockWindow;

        aDockingAreaRect = ::Rectangle( pDockingAreaWindow->GetPosPixel(), pDockingAreaWindow->GetSizePixel() );
        if ( pToolBox )
        {
            // docked toolbars always have one line
            ::Size aSize = pToolBox->CalcWindowSizePixel( 1, ImplConvertAlignment( sal_Int16( eDockedArea )) );
            aTrackingRect.SetSize( ::Size( aSize.Width(), aSize.Height() ));
        }
    }

    // default docking operation, dock on the given row/column
    sal_Bool                                 bOpOutsideOfDockingArea( !aDockingAreaRect.IsInside( rMousePos ));
    std::vector< SingleRowColumnWindowData > aRowColumnsWindowData;

    rDockingOperation = DOCKOP_ON_COLROW;
    implts_getDockingAreaElementInfos( eDockedArea, aRowColumnsWindowData );

    // determine current first row/column and last row/column
    sal_Int32 nMaxRowCol( -1 );
    sal_Int32 nMinRowCol( SAL_MAX_INT32 );
    const sal_uInt32 nCount = aRowColumnsWindowData.size();
    for ( sal_uInt32 i = 0; i < nCount; i++ )
    {
        if ( aRowColumnsWindowData[i].nRowColumn > nMaxRowCol )
            nMaxRowCol = aRowColumnsWindowData[i].nRowColumn;
        if ( aRowColumnsWindowData[i].nRowColumn < nMinRowCol )
            nMinRowCol = aRowColumnsWindowData[i].nRowColumn;
    }

    if ( !bOpOutsideOfDockingArea )
    {
        // docking inside our docking area
        sal_Int32   nIndex( -1 );
        sal_Int32   nRowCol( -1 );
        ::Rectangle aWindowRect;
        ::Rectangle aRowColumnRect;

        const sal_uInt32 nWindowDataCount = aRowColumnsWindowData.size();
        for ( sal_uInt32 i = 0; i < nWindowDataCount; i++ )
        {
            ::Rectangle aRect( aRowColumnsWindowData[i].aRowColumnRect.X,
                               aRowColumnsWindowData[i].aRowColumnRect.Y,
                               aRowColumnsWindowData[i].aRowColumnRect.X + aRowColumnsWindowData[i].aRowColumnRect.Width,
                               aRowColumnsWindowData[i].aRowColumnRect.Y + aRowColumnsWindowData[i].aRowColumnRect.Height );

            {
                // Calc correct position of the column/row rectangle to be able to compare it with mouse pos/tracking rect
                vos::OGuard aGuard( Application::GetSolarMutex() );
                aRect.SetPos( pContainerWindow->ScreenToOutputPixel( pDockingAreaWindow->OutputToScreenPixel( aRect.TopLeft() )));
            }

            sal_Bool bIsInsideRowCol( aRect.IsInside( rMousePos ) );
            if ( bIsInsideRowCol )
            {
                nIndex            = i;
                nRowCol           = aRowColumnsWindowData[i].nRowColumn;
                rDockingOperation = implts_determineDockingOperation( eDockedArea, aRect, rMousePos );
                aWindowRect       = implts_getWindowRectFromRowColumn( eDockedArea, aRowColumnsWindowData[i], rMousePos, rUIElement.m_aName );
                aRowColumnRect    = aRect;
                break;
            }
        }

        OSL_ENSURE( ( nIndex >= 0 ) && ( nRowCol >= 0 ), "Impossible case - no row/column found but mouse pointer is inside our docking area" );
        if (( nIndex >= 0 ) && ( nRowCol >= 0 ))
        {
            if ( rDockingOperation == DOCKOP_ON_COLROW )
            {
                if ( !aWindowRect.IsEmpty())
                {
                    // Tracking rect is on a row/column and mouse is over a docked toolbar.
                    // Determine if the tracking rect must be located before/after the docked toolbar.

                    ::Rectangle aUIElementRect( aWindowRect );
                    sal_Int32   nMiddle( bHorizontalDockArea ? ( aWindowRect.Left() + aWindowRect.getWidth() / 2 ) :
                                                               ( aWindowRect.Top() + aWindowRect.getHeight() / 2 ));
                    sal_Bool    bInsertBefore( bHorizontalDockArea ? ( rMousePos.X() < nMiddle ) : ( rMousePos.Y() < nMiddle ));
                    if ( bInsertBefore )
                    {
                        if ( bHorizontalDockArea )
                        {
                            sal_Int32 nSize = ::std::max( sal_Int32( 0 ), std::min( sal_Int32( aContainerWinSize.Width() -  aWindowRect.Left() ),
                                                                                    sal_Int32( aTrackingRect.getWidth() )));
                            if ( nSize == 0 )
                                nSize = aWindowRect.getWidth();

                            aUIElementRect.SetSize( ::Size( nSize, aWindowRect.getHeight() ));
                            aWindowRect = implts_determineFrontDockingRect( eDockedArea, nRowCol, aWindowRect,rUIElement.m_aName, aUIElementRect );

                            // Set virtual position
                            rUIElement.m_aDockedData.m_aPos.X() = aWindowRect.Left();
                            rUIElement.m_aDockedData.m_aPos.Y() = nRowCol;
                        }
                        else
                        {
                            sal_Int32 nSize = ::std::max( sal_Int32( 0 ), std::min( sal_Int32(
                                                    nTopDockingAreaSize + nMaxLeftRightDockAreaSize - aWindowRect.Top() ),
                                                    sal_Int32( aTrackingRect.getHeight() )));
                            if ( nSize == 0 )
                                nSize = aWindowRect.getHeight();

                            aUIElementRect.SetSize( ::Size( aWindowRect.getWidth(), nSize ));
                            aWindowRect = implts_determineFrontDockingRect( eDockedArea, nRowCol, aWindowRect, rUIElement.m_aName, aUIElementRect );

                            // Set virtual position
                            sal_Int32 nPosY = pDockingAreaWindow->ScreenToOutputPixel(
                                                pContainerWindow->OutputToScreenPixel( aWindowRect.TopLeft() )).Y();
                            rUIElement.m_aDockedData.m_aPos.X() = nRowCol;
                            rUIElement.m_aDockedData.m_aPos.Y() = nPosY;
                        }

                        rTrackingRect = aWindowRect;
                        return;
                    }
                    else
                    {
                        if ( bHorizontalDockArea )
                        {
                            sal_Int32 nSize = ::std::max( sal_Int32( 0 ), std::min( sal_Int32(( aContainerWinSize.Width() ) - aWindowRect.Right() ),
                                                                                    sal_Int32( aTrackingRect.getWidth() )));
                            if ( nSize == 0 )
                            {
                                aUIElementRect.SetPos( ::Point( aContainerWinSize.Width() - aTrackingRect.getWidth(), aWindowRect.Top() ));
                                aUIElementRect.SetSize( ::Size( aTrackingRect.getWidth(), aWindowRect.getHeight() ));
                                rUIElement.m_aDockedData.m_aPos.X() = aUIElementRect.Left();
                            }
                            else
                            {
                                aUIElementRect.SetPos( ::Point( aWindowRect.Right(), aWindowRect.Top() ));
                                aUIElementRect.SetSize( ::Size( nSize, aWindowRect.getHeight() ));
                                rUIElement.m_aDockedData.m_aPos.X() = aWindowRect.Right();
                            }

                            // Set virtual position
                            rUIElement.m_aDockedData.m_aPos.Y() = nRowCol;
                        }
                        else
                        {
                            sal_Int32 nSize = ::std::max( sal_Int32( 0 ), std::min( sal_Int32( nTopDockingAreaSize + nMaxLeftRightDockAreaSize - aWindowRect.Bottom() ),
                                                                                    sal_Int32( aTrackingRect.getHeight() )));
                            aUIElementRect.SetPos( ::Point( aWindowRect.Left(), aWindowRect.Bottom() ));
                            aUIElementRect.SetSize( ::Size( aWindowRect.getWidth(), nSize ));

                            // Set virtual position
                            sal_Int32 nPosY( 0 );
                            {
                                vos::OGuard aGuard( Application::GetSolarMutex() );
                                nPosY = pDockingAreaWindow->ScreenToOutputPixel(
                                                    pContainerWindow->OutputToScreenPixel( aWindowRect.BottomRight() )).Y();
                            }
                            rUIElement.m_aDockedData.m_aPos.X() = nRowCol;
                            rUIElement.m_aDockedData.m_aPos.Y() = nPosY;
                        }

                        rTrackingRect = aUIElementRect;
                        return;
                    }
                }
                else
                {
                    implts_setTrackingRect( eDockedArea, rMousePos, aTrackingRect );
                    rTrackingRect = implts_calcTrackingAndElementRect(
                                        eDockedArea, nRowCol, rUIElement,
                                        aTrackingRect, aRowColumnRect, aContainerWinSize );
                    return;
                }
            }
            else
            {
                if ((( nRowCol == nMinRowCol ) && ( rDockingOperation == DOCKOP_BEFORE_COLROW )) ||
                    (( nRowCol == nMaxRowCol ) && ( rDockingOperation == DOCKOP_AFTER_COLROW  )))
                    bOpOutsideOfDockingArea = sal_True;
                else
                {
                    // handle docking before/after a row
                    implts_setTrackingRect( eDockedArea, rMousePos, aTrackingRect );
                    rTrackingRect = implts_calcTrackingAndElementRect(
                                        eDockedArea, nRowCol, rUIElement,
                                        aTrackingRect, aRowColumnRect, aContainerWinSize );

                    sal_Int32 nOffsetX( 0 );
                    sal_Int32 nOffsetY( 0 );
                    if ( bHorizontalDockArea )
                        nOffsetY = sal_Int32( floor( aRowColumnRect.getHeight() / 2 + 0.5 ));
                    else
                        nOffsetX = sal_Int32( floor( aRowColumnRect.getWidth() / 2 + 0.5 ));

                    if ( rDockingOperation == DOCKOP_BEFORE_COLROW )
                    {
                        if (( eDockedArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
                            ( eDockedArea == ui::DockingArea_DOCKINGAREA_LEFT ))
                        {
                            // Docking before/after means move track rectangle half column/row.
                            // As left and top are ordered 0...n instead of right and bottom
                            // which uses n...0, we have to use negative values for top/left.
                            nOffsetX *= -1;
                            nOffsetY *= -1;
                        }
                    }
                    else
                    {
                        if (( eDockedArea == ui::DockingArea_DOCKINGAREA_BOTTOM ) ||
                            ( eDockedArea == ui::DockingArea_DOCKINGAREA_RIGHT ))
                        {
                            // Docking before/after means move track rectangle half column/row.
                            // As left and top are ordered 0...n instead of right and bottom
                            // which uses n...0, we have to use negative values for top/left.
                            nOffsetX *= -1;
                            nOffsetY *= -1;
                        }
                        nRowCol++;
                    }

                    if ( bHorizontalDockArea )
                        rUIElement.m_aDockedData.m_aPos.Y() = nRowCol;
                    else
                        rUIElement.m_aDockedData.m_aPos.X() = nRowCol;

                    rTrackingRect.Move( nOffsetX, nOffsetY );
                    rTrackingRect.SetSize( aTrackingRect.GetSize() );
                }
            }
        }
    }

    // Docking outside of our docking window area =>
    // Users want to dock before/after first/last docked element or to an empty docking area
    if ( bOpOutsideOfDockingArea )
    {
        // set correct size for docking
        implts_setTrackingRect( eDockedArea, rMousePos, aTrackingRect );
        rTrackingRect = aTrackingRect;

        if ( bHorizontalDockArea )
        {
            sal_Int32 nPosX( std::max( sal_Int32( rTrackingRect.Left()), sal_Int32( 0 )));
            if (( nPosX + rTrackingRect.getWidth()) > aContainerWinSize.Width() )
                nPosX = std::min( nPosX,
                                std::max( sal_Int32( aContainerWinSize.Width() - rTrackingRect.getWidth() ),
                                          sal_Int32( 0 )));

            sal_Int32 nSize = std::min( aContainerWinSize.Width(), rTrackingRect.getWidth() );
            sal_Int32 nDockHeight = std::max( static_cast<sal_Int32>(aDockingAreaRect.getHeight()), sal_Int32( 0 ));
            if ( nDockHeight == 0 )
            {
                sal_Int32 nPosY( std::max( aDockingAreaRect.Top(), aDockingAreaRect.Bottom() ));
                if ( eDockedArea == ui::DockingArea_DOCKINGAREA_BOTTOM )
                    nPosY -= rTrackingRect.getHeight();
                rTrackingRect.SetPos( Point( nPosX, nPosY ));
                rUIElement.m_aDockedData.m_aPos.Y() = 0;
            }
            else if ( rMousePos.Y() < ( aDockingAreaRect.Top() + ( nDockHeight / 2 )))
            {
                rTrackingRect.SetPos( Point( nPosX, aDockingAreaRect.Top() - rTrackingRect.getHeight() ));
                if ( eDockedArea == ui::DockingArea_DOCKINGAREA_TOP )
                    rUIElement.m_aDockedData.m_aPos.Y() = 0;
                else
                    rUIElement.m_aDockedData.m_aPos.Y() = ( nMaxRowCol >= 0 ) ? nMaxRowCol+1 : 0;
                rDockingOperation = DOCKOP_BEFORE_COLROW;
            }
            else
            {
                rTrackingRect.SetPos( Point( nPosX, aDockingAreaRect.Bottom() ));
                if ( eDockedArea == ui::DockingArea_DOCKINGAREA_TOP )
                    rUIElement.m_aDockedData.m_aPos.Y() = ( nMaxRowCol >= 0 ) ? nMaxRowCol+1 : 0;
                else
                    rUIElement.m_aDockedData.m_aPos.Y() = 0;
                rDockingOperation = DOCKOP_AFTER_COLROW;
            }
            rTrackingRect.setWidth( nSize );

            {
                vos::OGuard aGuard( Application::GetSolarMutex() );
                nPosX = pDockingAreaWindow->ScreenToOutputPixel(
                                    pContainerWindow->OutputToScreenPixel( rTrackingRect.TopLeft() )).X();
            }
            rUIElement.m_aDockedData.m_aPos.X() = nPosX;
        }
        else
        {
            sal_Int32 nMaxDockingAreaHeight = std::max( sal_Int32( 0 ),
                                                        sal_Int32( nMaxLeftRightDockAreaSize ));

            sal_Int32 nPosY( std::max( sal_Int32( aTrackingRect.Top()), sal_Int32( nTopDockingAreaSize )));
            if (( nPosY + aTrackingRect.getHeight()) > ( nTopDockingAreaSize + nMaxDockingAreaHeight ))
                nPosY = std::min( nPosY,
                                std::max( sal_Int32( nTopDockingAreaSize + ( nMaxDockingAreaHeight - aTrackingRect.getHeight() )),
                                        sal_Int32( nTopDockingAreaSize )));

            sal_Int32 nSize = std::min( nMaxDockingAreaHeight, static_cast<sal_Int32>(aTrackingRect.getHeight()) );
            sal_Int32 nDockWidth = std::max( static_cast<sal_Int32>(aDockingAreaRect.getWidth()), sal_Int32( 0 ));
            if ( nDockWidth == 0 )
            {
                sal_Int32 nPosX( std::max( aDockingAreaRect.Left(), aDockingAreaRect.Right() ));
                if ( eDockedArea == ui::DockingArea_DOCKINGAREA_RIGHT )
                    nPosX -= rTrackingRect.getWidth();
                rTrackingRect.SetPos( Point( nPosX, nPosY ));
                rUIElement.m_aDockedData.m_aPos.X() = 0;
            }
            else if ( rMousePos.X() < ( aDockingAreaRect.Left() + ( nDockWidth / 2 )))
            {
                rTrackingRect.SetPos( Point( aDockingAreaRect.Left() - rTrackingRect.getWidth(), nPosY ));
                if ( eDockedArea == ui::DockingArea_DOCKINGAREA_LEFT )
                    rUIElement.m_aDockedData.m_aPos.X() = 0;
                else
                    rUIElement.m_aDockedData.m_aPos.X() = ( nMaxRowCol >= 0 ) ? nMaxRowCol+1 : 0;
                rDockingOperation = DOCKOP_BEFORE_COLROW;
            }
            else
            {
                rTrackingRect.SetPos( Point( aDockingAreaRect.Right(), nPosY ));
                if ( eDockedArea == ui::DockingArea_DOCKINGAREA_LEFT )
                    rUIElement.m_aDockedData.m_aPos.X() = ( nMaxRowCol >= 0 ) ? nMaxRowCol+1 : 0;
                else
                    rUIElement.m_aDockedData.m_aPos.X() = 0;
                rDockingOperation = DOCKOP_AFTER_COLROW;
            }
            rTrackingRect.setHeight( nSize );

            {
                vos::OGuard aGuard( Application::GetSolarMutex() );
                nPosY = pDockingAreaWindow->ScreenToOutputPixel(
                                    pContainerWindow->OutputToScreenPixel( rTrackingRect.TopLeft() )).Y();
            }
            rUIElement.m_aDockedData.m_aPos.Y() = nPosY;
        }
    }
}

framework::ToolbarLayoutManager::DockingOperation ToolbarLayoutManager::implts_determineDockingOperation(
    ui::DockingArea    DockingArea,
    const ::Rectangle& rRowColRect,
    const Point&       rMousePos )
{
    const sal_Int32 nHorzVerticalRegionSize        = 6;
    const sal_Int32 nHorzVerticalMoveRegion        = 4;

    if ( rRowColRect.IsInside( rMousePos ))
    {
        if (( DockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
            ( DockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ))
        {
            sal_Int32 nRegion = rRowColRect.getHeight() / nHorzVerticalRegionSize;
            sal_Int32 nPosY   = rRowColRect.Top() + nRegion;

            if ( rMousePos.Y() < nPosY )
                return ( DockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ? DOCKOP_BEFORE_COLROW : DOCKOP_AFTER_COLROW;
            else if ( rMousePos.Y() < ( nPosY + nRegion*nHorzVerticalMoveRegion ))
                return DOCKOP_ON_COLROW;
            else
                return ( DockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ? DOCKOP_AFTER_COLROW : DOCKOP_BEFORE_COLROW;
        }
        else
        {
            sal_Int32 nRegion = rRowColRect.getWidth() / nHorzVerticalRegionSize;
            sal_Int32 nPosX   = rRowColRect.Left() + nRegion;

            if ( rMousePos.X() < nPosX )
                return ( DockingArea == ui::DockingArea_DOCKINGAREA_LEFT ) ? DOCKOP_BEFORE_COLROW : DOCKOP_AFTER_COLROW;
            else if ( rMousePos.X() < ( nPosX + nRegion*nHorzVerticalMoveRegion ))
                return DOCKOP_ON_COLROW;
            else
                return ( DockingArea == ui::DockingArea_DOCKINGAREA_LEFT ) ? DOCKOP_AFTER_COLROW : DOCKOP_BEFORE_COLROW;
        }
    }
    else
        return DOCKOP_ON_COLROW;
}

::Rectangle ToolbarLayoutManager::implts_calcTrackingAndElementRect(
    ui::DockingArea eDockingArea,
    sal_Int32 nRowCol,
    UIElement& rUIElement,
    const ::Rectangle& rTrackingRect,
    const ::Rectangle& rRowColumnRect,
    const ::Size& rContainerWinSize )
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadGuard( m_aLock );
    ::Rectangle aDockingAreaOffsets( m_aDockingAreaOffsets );
    aReadGuard.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    sal_Bool bHorizontalDockArea(( eDockingArea == ui::DockingArea_DOCKINGAREA_TOP    ) ||
                                 ( eDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ));

    sal_Int32 nTopDockingAreaSize( implts_getTopBottomDockingAreaSizes().Width() );
    sal_Int32 nBottomDockingAreaSize( implts_getTopBottomDockingAreaSizes().Height() );

    sal_Int32 nMaxLeftRightDockAreaSize = rContainerWinSize.Height() -
                                          nTopDockingAreaSize -
                                          nBottomDockingAreaSize -
                                          aDockingAreaOffsets.Top() -
                                          aDockingAreaOffsets.Bottom();

    ::Rectangle aTrackingRect( rTrackingRect );
    if ( bHorizontalDockArea )
    {
        sal_Int32 nPosX( std::max( sal_Int32( rTrackingRect.Left()), sal_Int32( 0 )));
        if (( nPosX + rTrackingRect.getWidth()) > rContainerWinSize.Width() )
            nPosX = std::min( nPosX,
                                std::max( sal_Int32( rContainerWinSize.Width() - rTrackingRect.getWidth() ),
                                          sal_Int32( 0 )));

        sal_Int32 nSize = std::min( rContainerWinSize.Width(), rTrackingRect.getWidth() );

        aTrackingRect.SetPos( ::Point( nPosX, rRowColumnRect.Top() ));
        aTrackingRect.setWidth( nSize );
        aTrackingRect.setHeight( rRowColumnRect.getHeight() );

        // Set virtual position
        rUIElement.m_aDockedData.m_aPos.X() = nPosX;
        rUIElement.m_aDockedData.m_aPos.Y() = nRowCol;
    }
    else
    {
        sal_Int32 nMaxDockingAreaHeight = std::max( sal_Int32( 0 ),
                                                    sal_Int32( nMaxLeftRightDockAreaSize ));

        sal_Int32 nPosY( std::max( sal_Int32( aTrackingRect.Top()), sal_Int32( nTopDockingAreaSize )));
        if (( nPosY + aTrackingRect.getHeight()) > ( nTopDockingAreaSize + nMaxDockingAreaHeight ))
            nPosY = std::min( nPosY,
                                std::max( sal_Int32( nTopDockingAreaSize + ( nMaxDockingAreaHeight - aTrackingRect.getHeight() )),
                                        sal_Int32( nTopDockingAreaSize )));

        sal_Int32 nSize = std::min( nMaxDockingAreaHeight, static_cast<sal_Int32>(aTrackingRect.getHeight()) );

        aTrackingRect.SetPos( ::Point( rRowColumnRect.Left(), nPosY ));
        aTrackingRect.setWidth( rRowColumnRect.getWidth() );
        aTrackingRect.setHeight( nSize );

        /* SAFE AREA ----------------------------------------------------------------------------------------------- */
        aReadGuard.lock();
        uno::Reference< awt::XWindow  > xDockingAreaWindow = m_xDockAreaWindows[eDockingArea];
        uno::Reference< awt::XWindow2 > xContainerWindow   = m_xContainerWindow;
        aReadGuard.unlock();
        /* SAFE AREA ----------------------------------------------------------------------------------------------- */

        sal_Int32 nDockPosY( 0 );
        Window* pDockingAreaWindow( 0 );
        Window* pContainerWindow( 0 );
        {
            vos::OGuard aGuard( Application::GetSolarMutex() );
            pDockingAreaWindow = VCLUnoHelper::GetWindow( xDockingAreaWindow );
            pContainerWindow = VCLUnoHelper::GetWindow( xContainerWindow );
            nDockPosY = pDockingAreaWindow->ScreenToOutputPixel(
                                pContainerWindow->OutputToScreenPixel( ::Point( 0, nPosY ))).Y();
        }

        // Set virtual position
        rUIElement.m_aDockedData.m_aPos.X() = nRowCol;
        rUIElement.m_aDockedData.m_aPos.Y() = nDockPosY;
    }

    return aTrackingRect;
}

void ToolbarLayoutManager::implts_setTrackingRect( ui::DockingArea eDockingArea, const ::Point& rMousePos, ::Rectangle& rTrackingRect )
{
    sal_Bool bHorizontalDockArea( ( eDockingArea == ui::DockingArea_DOCKINGAREA_TOP ) ||
        ( eDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ));

    ::Point aPoint = rTrackingRect.TopLeft();
    if ( bHorizontalDockArea )
        aPoint.X() = rMousePos.X();
    else
        aPoint.Y() = rMousePos.Y();

    rTrackingRect.SetPos( aPoint );
}

void ToolbarLayoutManager::implts_renumberRowColumnData(
    ui::DockingArea eDockingArea,
    DockingOperation /*eDockingOperation*/,
    const UIElement& rUIElement )
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    uno::Reference< container::XNameAccess > xPersistentWindowState( m_xPersistentWindowState );
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    sal_Bool  bHorzDockingArea(( eDockingArea == ui::DockingArea_DOCKINGAREA_TOP    ) ||
                               ( eDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM ));
    sal_Int32 nRowCol( bHorzDockingArea ? rUIElement.m_aDockedData.m_aPos.Y() :
                                          rUIElement.m_aDockedData.m_aPos.X() );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    UIElementVector::iterator pIter;
    for ( pIter = m_aUIElements.begin(); pIter != m_aUIElements.end(); pIter++ )
    {
        if (( pIter->m_aDockedData.m_nDockedArea == sal_Int16( eDockingArea )) &&
            ( pIter->m_aName != rUIElement.m_aName ))
        {
            // Don't change toolbars without a valid docking position!
            if (( pIter->m_aDockedData.m_aPos.X() == SAL_MAX_INT32 ) &&
                ( pIter->m_aDockedData.m_aPos.Y() == SAL_MAX_INT32 ))
                continue;

            sal_Int32 nWindowRowCol = ( bHorzDockingArea ) ?
                pIter->m_aDockedData.m_aPos.Y() : pIter->m_aDockedData.m_aPos.X();
            if ( nWindowRowCol >= nRowCol )
            {
                if ( bHorzDockingArea )
                    pIter->m_aDockedData.m_aPos.Y() += 1;
                else
                    pIter->m_aDockedData.m_aPos.X() += 1;
            }
        }
    }
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    // We have to change the persistent window state part
    if ( xPersistentWindowState.is() )
    {
        try
        {
            uno::Sequence< ::rtl::OUString > aWindowElements = xPersistentWindowState->getElementNames();
            for ( sal_Int32 i = 0; i < aWindowElements.getLength(); i++ )
            {
                if ( rUIElement.m_aName != aWindowElements[i] )
                {
                    try
                    {
                        uno::Sequence< beans::PropertyValue > aPropValueSeq;
                        awt::Point                            aDockedPos;
                        ui::DockingArea                       nDockedArea( ui::DockingArea_DOCKINGAREA_DEFAULT );

                        xPersistentWindowState->getByName( aWindowElements[i] ) >>= aPropValueSeq;
                        for ( sal_Int32 j = 0; j < aPropValueSeq.getLength(); j++ )
                        {
                            if ( aPropValueSeq[j].Name.equalsAscii( WINDOWSTATE_PROPERTY_DOCKINGAREA ))
                                aPropValueSeq[j].Value >>= nDockedArea;
                            else if ( aPropValueSeq[j].Name.equalsAscii( WINDOWSTATE_PROPERTY_DOCKPOS ))
                                aPropValueSeq[j].Value >>= aDockedPos;
                        }

                        // Don't change toolbars without a valid docking position!
                        if (( aDockedPos.X == SAL_MAX_INT32 ) && ( aDockedPos.Y == SAL_MAX_INT32 ))
                            continue;

                        sal_Int32 nWindowRowCol = ( bHorzDockingArea ) ? aDockedPos.Y : aDockedPos.X;
                        if (( nDockedArea == eDockingArea ) && ( nWindowRowCol >= nRowCol ))
                        {
                            if ( bHorzDockingArea )
                                aDockedPos.Y += 1;
                            else
                                aDockedPos.X += 1;

                            uno::Reference< container::XNameReplace > xReplace( xPersistentWindowState, uno::UNO_QUERY );
                            xReplace->replaceByName( aWindowElements[i], makeAny( aPropValueSeq ));
                        }
                    }
                    catch ( uno::Exception& )
                    {
                    }
                }
            }
        }
        catch ( uno::Exception& )
        {
        }
    }
}

//---------------------------------------------------------------------------------------------------------
//  XWindowListener
//---------------------------------------------------------------------------------------------------------
void SAL_CALL ToolbarLayoutManager::windowResized( const css::awt::WindowEvent& aEvent )
throw( css::uno::RuntimeException )
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    bool bLocked( m_bDockingInProgress );
    bool bDoLayout( m_bLayoutInProgress );
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    // Do not do anything if we are in the middle of a docking process. This would interfere all other
    // operations. We will store the new position and size in the docking handlers.
    // Do not do anything if we are in the middle of our layouting process. We will adapt the position
    // and size of the user interface elements.
    if ( !bLocked && !bDoLayout )
    {
        bool                                bNotify( false );
        uno::Reference< css::awt::XWindow > xWindow( aEvent.Source, uno::UNO_QUERY );

        UIElement aUIElement = implts_findToolbar( aEvent.Source );
        if ( aUIElement.m_xUIElement.is() )
        {
            if ( aUIElement.m_bFloating )
            {
                uno::Reference< css::awt::XWindow2 > xWindow2( xWindow, uno::UNO_QUERY );

                if( xWindow2.is() )
                {
                    awt::Rectangle aPos     = xWindow2->getPosSize();
                    awt::Size      aSize    = xWindow2->getOutputSize();   // always use output size for consistency
                    bool           bVisible = xWindow2->isVisible();

                    // update element data
                    aUIElement.m_aFloatingData.m_aPos  = ::Point( aPos.X, aPos.Y );
                    aUIElement.m_aFloatingData.m_aSize = ::Size( aSize.Width, aSize.Height );
                    aUIElement.m_bVisible              = bVisible;
                }

                implts_writeWindowStateData( aUIElement );
            }
            else
            {
                /* SAFE AREA ----------------------------------------------------------------------------------------------- */
                aWriteLock.lock();
                m_bLayoutDirty = true;
                bNotify        = true;
                aWriteLock.unlock();
                /* SAFE AREA ----------------------------------------------------------------------------------------------- */
            }
        }

        if ( bNotify )
            m_pParentLayouter->requestLayout( ILayoutNotifications::HINT_TOOLBARSPACE_HAS_CHANGED );
    }
}

void SAL_CALL ToolbarLayoutManager::windowMoved( const css::awt::WindowEvent& /*aEvent*/ )
throw( css::uno::RuntimeException )
{
}

void SAL_CALL ToolbarLayoutManager::windowShown( const css::lang::EventObject& /*aEvent*/ )
throw( css::uno::RuntimeException )
{
}

void SAL_CALL ToolbarLayoutManager::windowHidden( const css::lang::EventObject& /*aEvent*/ )
throw( css::uno::RuntimeException )
{
}

//---------------------------------------------------------------------------------------------------------
//  XDockableWindowListener
//---------------------------------------------------------------------------------------------------------
void SAL_CALL ToolbarLayoutManager::startDocking(
    const awt::DockingEvent& e )
throw (uno::RuntimeException)
{
    bool bWinFound( false );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadGuard( m_aLock );
    uno::Reference< css::awt::XWindow2 > xContainerWindow( m_xContainerWindow );
    uno::Reference< css::awt::XWindow2 > xWindow( e.Source, uno::UNO_QUERY );
    aReadGuard.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    Window* pContainerWindow( 0 );
    Window* pWindow( 0 );
    ::Point aMousePos;
    {
        vos::OGuard aGuard( Application::GetSolarMutex() );
        pContainerWindow = VCLUnoHelper::GetWindow( xContainerWindow );
        aMousePos = pContainerWindow->ScreenToOutputPixel( ::Point( e.MousePos.X, e.MousePos.Y ));
    }

    UIElement aUIElement = implts_findToolbar( e.Source );

    if ( aUIElement.m_xUIElement.is() && xWindow.is() )
    {
        awt::Rectangle aRect;

        bWinFound = true;
        uno::Reference< css::awt::XDockableWindow > xDockWindow( xWindow, uno::UNO_QUERY );
        if ( xDockWindow->isFloating() )
        {
            awt::Rectangle aPos  = xWindow->getPosSize();
            awt::Size      aSize = xWindow->getOutputSize();

            aUIElement.m_aFloatingData.m_aPos    = ::Point( aPos.X, aPos.Y );
            aUIElement.m_aFloatingData.m_aSize   = ::Size( aSize.Width, aSize.Height );

            vos::OGuard aGuard( Application::GetSolarMutex() );
            pWindow = VCLUnoHelper::GetWindow( xWindow );
            if ( pWindow && pWindow->GetType() == WINDOW_TOOLBOX )
            {
                ToolBox* pToolBox = (ToolBox *)pWindow;
                aUIElement.m_aFloatingData.m_nLines          = pToolBox->GetFloatingLines();
                aUIElement.m_aFloatingData.m_bIsHorizontal   = (( pToolBox->GetAlign() == WINDOWALIGN_TOP ) ||
                                                                ( pToolBox->GetAlign() == WINDOWALIGN_BOTTOM ));
            }
        }
    }

    WriteGuard aWriteLock( m_aLock );
    m_bDockingInProgress = bWinFound;
    m_aDockUIElement = aUIElement;
    m_aDockUIElement.m_bUserActive = sal_True;
    m_aStartDockMousePos = aMousePos;
    aWriteLock.unlock();
}

awt::DockingData SAL_CALL ToolbarLayoutManager::docking( const awt::DockingEvent& e )
throw (uno::RuntimeException)
{
    const sal_Int32 MAGNETIC_DISTANCE_UNDOCK = 25;
    const sal_Int32 MAGNETIC_DISTANCE_DOCK   = 20;

    awt::DockingData                       aDockingData;
    uno::Reference< awt::XDockableWindow > xDockWindow( e.Source, uno::UNO_QUERY );
    uno::Reference< awt::XWindow >         xWindow( e.Source, uno::UNO_QUERY );
    uno::Reference< awt::XWindow >         xTopDockingWindow;
    uno::Reference< awt::XWindow >         xLeftDockingWindow;
    uno::Reference< awt::XWindow >         xRightDockingWindow;
    uno::Reference< awt::XWindow >         xBottomDockingWindow;
    uno::Reference< awt::XWindow2 >        xContainerWindow;
    UIElement                              aUIDockingElement;
    DockingOperation                       eDockingOperation( DOCKOP_ON_COLROW );

    sal_Bool bDockingInProgress;

    {
        /* SAFE AREA ----------------------------------------------------------------------------------------------- */
        ReadGuard aReadLock( m_aLock );
        bDockingInProgress = m_bDockingInProgress;
        if ( bDockingInProgress )
        {
            xContainerWindow     = m_xContainerWindow;
            xTopDockingWindow    = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_TOP];
            xLeftDockingWindow   = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_LEFT];
            xRightDockingWindow  = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_RIGHT];
            xBottomDockingWindow = m_xDockAreaWindows[ui::DockingArea_DOCKINGAREA_BOTTOM];
            aUIDockingElement    = m_aDockUIElement;

            aDockingData.TrackingRectangle = e.TrackingRectangle;
        }
        /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    }

    if ( bDockingInProgress && xDockWindow.is() && xWindow.is() )
    {
        try
        {
            vos::OGuard aGuard( Application::GetSolarMutex() );

            sal_Int16 eDockingArea( -1 ); // none
            sal_Int32 nMagneticZone( aUIDockingElement.m_bFloating ? MAGNETIC_DISTANCE_DOCK : MAGNETIC_DISTANCE_UNDOCK );
            awt::Rectangle aNewTrackingRect;
            ::Rectangle aTrackingRect( e.TrackingRectangle.X,
                                       e.TrackingRectangle.Y,
                                       ( e.TrackingRectangle.X + e.TrackingRectangle.Width ),
                                       ( e.TrackingRectangle.Y + e.TrackingRectangle.Height ));

            awt::Rectangle aTmpRect = xTopDockingWindow->getPosSize();
            ::Rectangle aTopDockRect( aTmpRect.X, aTmpRect.Y, aTmpRect.Width, aTmpRect.Height );
            ::Rectangle aHotZoneTopDockRect( implts_calcHotZoneRect( aTopDockRect, nMagneticZone ));

            aTmpRect = xBottomDockingWindow->getPosSize();
            ::Rectangle aBottomDockRect( aTmpRect.X, aTmpRect.Y, ( aTmpRect.X + aTmpRect.Width), ( aTmpRect.Y + aTmpRect.Height ));
            ::Rectangle aHotZoneBottomDockRect( implts_calcHotZoneRect( aBottomDockRect, nMagneticZone ));

            aTmpRect = xLeftDockingWindow->getPosSize();
            ::Rectangle aLeftDockRect( aTmpRect.X, aTmpRect.Y, ( aTmpRect.X + aTmpRect.Width ), ( aTmpRect.Y + aTmpRect.Height ));
            ::Rectangle aHotZoneLeftDockRect( implts_calcHotZoneRect( aLeftDockRect, nMagneticZone ));

            aTmpRect = xRightDockingWindow->getPosSize();
            ::Rectangle aRightDockRect( aTmpRect.X, aTmpRect.Y, ( aTmpRect.X + aTmpRect.Width ), ( aTmpRect.Y + aTmpRect.Height ));
            ::Rectangle aHotZoneRightDockRect( implts_calcHotZoneRect( aRightDockRect, nMagneticZone ));

            Window* pContainerWindow( VCLUnoHelper::GetWindow( xContainerWindow ) );
            Window* pDockingAreaWindow( 0 );
            ::Point aMousePos( pContainerWindow->ScreenToOutputPixel( ::Point( e.MousePos.X, e.MousePos.Y )));

            if ( aHotZoneTopDockRect.IsInside( aMousePos ))
                eDockingArea = ui::DockingArea_DOCKINGAREA_TOP;
            else if ( aHotZoneBottomDockRect.IsInside( aMousePos ))
                eDockingArea = ui::DockingArea_DOCKINGAREA_BOTTOM;
            else if ( aHotZoneLeftDockRect.IsInside( aMousePos ))
                eDockingArea = ui::DockingArea_DOCKINGAREA_LEFT;
            else if ( aHotZoneRightDockRect.IsInside( aMousePos ))
                eDockingArea = ui::DockingArea_DOCKINGAREA_RIGHT;

            // Higher priority for movements inside the real docking area
            if ( aTopDockRect.IsInside( aMousePos ))
                eDockingArea = ui::DockingArea_DOCKINGAREA_TOP;
            else if ( aBottomDockRect.IsInside( aMousePos ))
                eDockingArea = ui::DockingArea_DOCKINGAREA_BOTTOM;
            else if ( aLeftDockRect.IsInside( aMousePos ))
                eDockingArea = ui::DockingArea_DOCKINGAREA_LEFT;
            else if ( aRightDockRect.IsInside( aMousePos ))
                eDockingArea = ui::DockingArea_DOCKINGAREA_RIGHT;

            // Determine if we have a toolbar and set alignment according to the docking area!
            Window*  pWindow = VCLUnoHelper::GetWindow( xWindow );
            ToolBox* pToolBox = 0;
            if ( pWindow && pWindow->GetType() == WINDOW_TOOLBOX )
                pToolBox = (ToolBox *)pWindow;

            if ( eDockingArea != -1 )
            {
                if ( eDockingArea == ui::DockingArea_DOCKINGAREA_TOP )
                {
                    aUIDockingElement.m_aDockedData.m_nDockedArea = ui::DockingArea_DOCKINGAREA_TOP;
                    aUIDockingElement.m_bFloating = sal_False;
                    pDockingAreaWindow = VCLUnoHelper::GetWindow( xTopDockingWindow );
                }
                else if ( eDockingArea == ui::DockingArea_DOCKINGAREA_BOTTOM )
                {
                    aUIDockingElement.m_aDockedData.m_nDockedArea = ui::DockingArea_DOCKINGAREA_BOTTOM;
                    aUIDockingElement.m_bFloating = sal_False;
                    pDockingAreaWindow = VCLUnoHelper::GetWindow( xBottomDockingWindow );
                }
                else if ( eDockingArea == ui::DockingArea_DOCKINGAREA_LEFT )
                {
                    aUIDockingElement.m_aDockedData.m_nDockedArea = ui::DockingArea_DOCKINGAREA_LEFT;
                    aUIDockingElement.m_bFloating = sal_False;
                    pDockingAreaWindow = VCLUnoHelper::GetWindow( xLeftDockingWindow );
                }
                else if ( eDockingArea == ui::DockingArea_DOCKINGAREA_RIGHT )
                {
                    aUIDockingElement.m_aDockedData.m_nDockedArea = ui::DockingArea_DOCKINGAREA_RIGHT;
                    aUIDockingElement.m_bFloating = sal_False;
                    pDockingAreaWindow = VCLUnoHelper::GetWindow( xRightDockingWindow );
                }

                ::Point aOutputPos = pContainerWindow->ScreenToOutputPixel( aTrackingRect.TopLeft() );
                aTrackingRect.SetPos( aOutputPos );

                ::Rectangle         aNewDockingRect( aTrackingRect );
                implts_calcDockingPosSize( aUIDockingElement, eDockingOperation, aNewDockingRect, aMousePos );

                ::Point aScreenPos = pContainerWindow->OutputToScreenPixel( aNewDockingRect.TopLeft() );
                aNewTrackingRect = css::awt::Rectangle( aScreenPos.X(),
                                                        aScreenPos.Y(),
                                                        aNewDockingRect.getWidth(),
                                                        aNewDockingRect.getHeight() );
                aDockingData.TrackingRectangle = aNewTrackingRect;
            }
            else if ( pToolBox && bDockingInProgress )
            {
                sal_Bool bIsHorizontal = (( pToolBox->GetAlign() == WINDOWALIGN_TOP    ) ||
                                          ( pToolBox->GetAlign() == WINDOWALIGN_BOTTOM ));
                ::Size aFloatSize = aUIDockingElement.m_aFloatingData.m_aSize;
                if ( aFloatSize.Width() > 0 && aFloatSize.Height() > 0 )
                {
                    aUIDockingElement.m_aFloatingData.m_aPos = pContainerWindow->ScreenToOutputPixel(
                                                                ::Point( e.MousePos.X, e.MousePos.Y ));
                    aDockingData.TrackingRectangle.Height = aFloatSize.Height();
                    aDockingData.TrackingRectangle.Width  = aFloatSize.Width();
                }
                else
                {
                    aFloatSize = pToolBox->CalcWindowSizePixel();
                    if ( !bIsHorizontal )
                    {
                        // Floating toolbars are always horizontal aligned! We have to swap
                        // width/height if we have a vertical aligned toolbar.
                        sal_Int32 nTemp = aFloatSize.Height();
                        aFloatSize.Height() = aFloatSize.Width();
                        aFloatSize.Width()  = nTemp;
                    }

                    aDockingData.TrackingRectangle.Height = aFloatSize.Height();
                    aDockingData.TrackingRectangle.Width  = aFloatSize.Width();

                    // For the first time we don't have any data about the floating size of a toolbar.
                    // We calculate it and store it for later use.
                    aUIDockingElement.m_aFloatingData.m_aPos = pContainerWindow->ScreenToOutputPixel(
                                                                ::Point( e.MousePos.X, e.MousePos.Y ));
                    aUIDockingElement.m_aFloatingData.m_aSize         = aFloatSize;
                    aUIDockingElement.m_aFloatingData.m_nLines        = pToolBox->GetFloatingLines();
                    aUIDockingElement.m_aFloatingData.m_bIsHorizontal = (( pToolBox->GetAlign() == WINDOWALIGN_TOP ) ||
                                                                         ( pToolBox->GetAlign() == WINDOWALIGN_BOTTOM ));
                }
                aDockingData.TrackingRectangle.X = e.MousePos.X;
                aDockingData.TrackingRectangle.Y = e.MousePos.Y;
            }

            aDockingData.bFloating = ( eDockingArea == -1 );

            // Write current data to the member docking progress data
            /* SAFE AREA ----------------------------------------------------------------------------------------------- */
            WriteGuard aWriteLock( m_aLock );
            m_aDockUIElement.m_bFloating = aDockingData.bFloating;
            if ( !aDockingData.bFloating )
            {
                m_aDockUIElement.m_aDockedData   = aUIDockingElement.m_aDockedData;
                m_eDockOperation                 = eDockingOperation;
            }
            else
                m_aDockUIElement.m_aFloatingData = aUIDockingElement.m_aFloatingData;
            aWriteLock.unlock();
            /* SAFE AREA ----------------------------------------------------------------------------------------------- */
        }
        catch ( uno::Exception& )
        {
        }
    }

    return aDockingData;
}

void SAL_CALL ToolbarLayoutManager::endDocking( const awt::EndDockingEvent& e )
throw (uno::RuntimeException)
{
    bool  bDockingInProgress( false );
    bool  bStartDockFloated( false );
    bool  bFloating( false );
    UIElement aUIDockingElement;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    bDockingInProgress = m_bDockingInProgress;
    aUIDockingElement  = m_aDockUIElement;
    bFloating          = aUIDockingElement.m_bFloating;

    UIElement& rUIElement = impl_findToolbar( aUIDockingElement.m_aName );
    if ( rUIElement.m_aName == aUIDockingElement.m_aName )
    {
        if ( aUIDockingElement.m_bFloating )
        {
            // Write last position into position data
            uno::Reference< awt::XWindow > xWindow( aUIDockingElement.m_xUIElement->getRealInterface(), uno::UNO_QUERY );
            rUIElement.m_aFloatingData = aUIDockingElement.m_aFloatingData;
            awt::Rectangle aTmpRect = xWindow->getPosSize();
            rUIElement.m_aFloatingData.m_aPos = ::Point( aTmpRect.X, aTmpRect.Y );
            // make changes also for our local data as we use it to make data persistent
            aUIDockingElement.m_aFloatingData = rUIElement.m_aFloatingData;
        }
        else
        {
            rUIElement.m_aDockedData = aUIDockingElement.m_aDockedData;
            rUIElement.m_aFloatingData.m_aSize = aUIDockingElement.m_aFloatingData.m_aSize;

            if ( m_eDockOperation != DOCKOP_ON_COLROW )
            {
                // we have to renumber our row/column data to insert a new row/column
                implts_renumberRowColumnData( (::com::sun::star::ui::DockingArea)aUIDockingElement.m_aDockedData.m_nDockedArea,
                                              m_eDockOperation,
                                              aUIDockingElement );
            }
        }

        bStartDockFloated        = rUIElement.m_bFloating;
        rUIElement.m_bFloating   = m_aDockUIElement.m_bFloating;
        rUIElement.m_bUserActive = sal_True;
    }

    // reset member for next docking operation
    m_aDockUIElement.m_xUIElement.clear();
    m_eDockOperation = DOCKOP_ON_COLROW;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    implts_writeWindowStateData( aUIDockingElement );

    if ( bDockingInProgress )
    {
        vos::OGuard aGuard( Application::GetSolarMutex() );
        Window*  pWindow = VCLUnoHelper::GetWindow( uno::Reference< awt::XWindow >( e.Source, uno::UNO_QUERY ));
        ToolBox* pToolBox = 0;
        if ( pWindow && pWindow->GetType() == WINDOW_TOOLBOX )
            pToolBox = (ToolBox *)pWindow;

        if ( pToolBox )
        {
            if( e.bFloating )
            {
                if ( aUIDockingElement.m_aFloatingData.m_bIsHorizontal )
                    pToolBox->SetAlign( WINDOWALIGN_TOP );
                else
                    pToolBox->SetAlign( WINDOWALIGN_LEFT );
            }
            else
            {
                ::Size aSize;

                pToolBox->SetAlign( ImplConvertAlignment( aUIDockingElement.m_aDockedData.m_nDockedArea) );

                // Docked toolbars have always one line
                aSize = pToolBox->CalcWindowSizePixel( 1 );

                // Lock layouting updates as our listener would be called due to SetSizePixel
                pToolBox->SetOutputSizePixel( aSize );
            }
        }
    }

    implts_sortUIElements();

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    aWriteLock.lock();
    m_bDockingInProgress = sal_False;
    m_bLayoutDirty       = !bStartDockFloated || !bFloating;
    bool bNotify         = m_bLayoutDirty;
    aWriteLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    if ( bNotify )
        m_pParentLayouter->requestLayout( ILayoutNotifications::HINT_TOOLBARSPACE_HAS_CHANGED );
}

sal_Bool SAL_CALL ToolbarLayoutManager::prepareToggleFloatingMode( const lang::EventObject& e )
throw (uno::RuntimeException)
{
    sal_Bool    bDockingInProgress( sal_False );

    ReadGuard aReadLock( m_aLock );
    bDockingInProgress = m_bDockingInProgress;
    aReadLock.unlock();

    UIElement aUIDockingElement = implts_findToolbar( e.Source );
    bool      bWinFound( aUIDockingElement.m_aName.getLength() > 0 );
    uno::Reference< awt::XWindow > xWindow( e.Source, uno::UNO_QUERY );

    if ( bWinFound && xWindow.is() )
    {
        if ( !bDockingInProgress )
        {
            awt::Rectangle aRect;
            uno::Reference< awt::XDockableWindow > xDockWindow( xWindow, uno::UNO_QUERY );
            if ( xDockWindow->isFloating() )
            {
                {
                    vos::OGuard aGuard( Application::GetSolarMutex() );
                    Window* pWindow = VCLUnoHelper::GetWindow( xWindow );
                    if ( pWindow )
                    {
                        if ( pWindow->GetType() == WINDOW_TOOLBOX )
                        {
                            ToolBox* pToolBox = (ToolBox *)pWindow;
                            aUIDockingElement.m_aFloatingData.m_aPos            = pToolBox->GetPosPixel();
                            aUIDockingElement.m_aFloatingData.m_aSize           = pToolBox->GetOutputSizePixel();
                            aUIDockingElement.m_aFloatingData.m_nLines          = pToolBox->GetFloatingLines();
                            aUIDockingElement.m_aFloatingData.m_bIsHorizontal   = (( pToolBox->GetAlign() == WINDOWALIGN_TOP ) ||
                                                                                   ( pToolBox->GetAlign() == WINDOWALIGN_BOTTOM ));
                        }
                    }
                }

                UIElement aUIElement = implts_findToolbar( aUIDockingElement.m_aName );
                if ( aUIElement.m_aName == aUIDockingElement.m_aName )
                    implts_setToolbar( aUIDockingElement );
            }
        }
    }

    return sal_True;
}

void SAL_CALL ToolbarLayoutManager::toggleFloatingMode(
    const lang::EventObject& e )
throw (uno::RuntimeException)
{
    sal_Bool  bDockingInProgress( sal_False );
    UIElement aUIDockingElement;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    bDockingInProgress = m_bDockingInProgress;
    if ( bDockingInProgress )
        aUIDockingElement = m_aDockUIElement;
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    Window*  pWindow( 0 );
    ToolBox* pToolBox( 0 );
    uno::Reference< css::awt::XWindow2 > xWindow;

    {
        vos::OGuard aGuard( Application::GetSolarMutex() );
        xWindow = uno::Reference< awt::XWindow2 >( e.Source, uno::UNO_QUERY );
        pWindow = VCLUnoHelper::GetWindow( xWindow );

        if ( pWindow && pWindow->GetType() == WINDOW_TOOLBOX )
            pToolBox = (ToolBox *)pWindow;
    }

    if ( !bDockingInProgress )
    {
        aUIDockingElement = implts_findToolbar( e.Source );
        bool bWinFound = ( aUIDockingElement.m_aName.getLength() > 0 );

        if ( bWinFound && xWindow.is() )
        {
            aUIDockingElement.m_bFloating   = !aUIDockingElement.m_bFloating;
            aUIDockingElement.m_bUserActive = sal_True;

            /* SAFE AREA ----------------------------------------------------------------------------------------------- */
            WriteGuard aWriteLock( m_aLock );
            m_bLayoutDirty = true;
            aWriteLock.unlock();
            /* SAFE AREA ----------------------------------------------------------------------------------------------- */

            if ( aUIDockingElement.m_bFloating )
            {
                vos::OGuard aGuard( Application::GetSolarMutex() );
                if ( pToolBox )
                {
                    pToolBox->SetLineCount( aUIDockingElement.m_aFloatingData.m_nLines );
                    if ( aUIDockingElement.m_aFloatingData.m_bIsHorizontal )
                        pToolBox->SetAlign( WINDOWALIGN_TOP );
                    else
                        pToolBox->SetAlign( WINDOWALIGN_LEFT );
                }

                sal_Bool bUndefPos = ( aUIDockingElement.m_aFloatingData.m_aPos.X() == SAL_MAX_INT32 ||
                                       aUIDockingElement.m_aFloatingData.m_aPos.Y() == SAL_MAX_INT32 );
                sal_Bool bSetSize = ( aUIDockingElement.m_aFloatingData.m_aSize.Width() != 0 &&
                                      aUIDockingElement.m_aFloatingData.m_aSize.Height() != 0 );

                if ( bUndefPos )
                    aUIDockingElement.m_aFloatingData.m_aPos = implts_findNextCascadeFloatingPos();

                if ( !bSetSize )
                {
                    if ( pToolBox )
                        aUIDockingElement.m_aFloatingData.m_aSize = pToolBox->CalcFloatingWindowSizePixel();
                    else
                        aUIDockingElement.m_aFloatingData.m_aSize = pWindow->GetOutputSizePixel();
                }

                xWindow->setPosSize( aUIDockingElement.m_aFloatingData.m_aPos.X(),
                                     aUIDockingElement.m_aFloatingData.m_aPos.Y(),
                                     0, 0, css::awt::PosSize::POS );
                xWindow->setOutputSize( AWTSize( aUIDockingElement.m_aFloatingData.m_aSize ) );
            }
            else
            {
                if (( aUIDockingElement.m_aDockedData.m_aPos.X() == SAL_MAX_INT32 ) &&
                    ( aUIDockingElement.m_aDockedData.m_aPos.Y() == SAL_MAX_INT32 ))
                {
                    // Docking on its default position without a preset position -
                    // we have to find a good place for it.
                    ::Point aPixelPos;
                    ::Point aDockPos;
                    ::Size  aSize;

                    {
                        vos::OGuard aGuard( Application::GetSolarMutex() );
                        if ( pToolBox )
                            aSize = pToolBox->CalcWindowSizePixel( 1, ImplConvertAlignment( aUIDockingElement.m_aDockedData.m_nDockedArea ) );
                        else
                            aSize = pWindow->GetSizePixel();
                    }

                    implts_findNextDockingPos( (::com::sun::star::ui::DockingArea)aUIDockingElement.m_aDockedData.m_nDockedArea,
                                                aSize,
                                                aDockPos,
                                                aPixelPos );
                    aUIDockingElement.m_aDockedData.m_aPos = aDockPos;
                }

                vos::OGuard aGuard( Application::GetSolarMutex() );
                if ( pToolBox )
                {
                    pToolBox->SetAlign( ImplConvertAlignment( aUIDockingElement.m_aDockedData.m_nDockedArea) );
                    ::Size aSize = pToolBox->CalcWindowSizePixel( 1 );
                    css::awt::Rectangle aRect = xWindow->getPosSize();
                    xWindow->setPosSize( aRect.X, aRect.Y, 0, 0, css::awt::PosSize::POS );
                    xWindow->setOutputSize( AWTSize( aSize ) );
                }
            }

            aWriteLock.lock();
            UIElement& rUIElement = impl_findToolbar( aUIDockingElement.m_aName );
            if ( rUIElement.m_aName == aUIDockingElement.m_aName )
                rUIElement = aUIDockingElement;
            m_bLayoutDirty = true;
            aWriteLock.unlock();

            implts_writeWindowStateData( aUIDockingElement );
            implts_sortUIElements();
        }
    }
    else
    {
        vos::OGuard aGuard( Application::GetSolarMutex() );
        if ( pToolBox )
        {
            if ( aUIDockingElement.m_bFloating )
            {
                if ( aUIDockingElement.m_aFloatingData.m_bIsHorizontal )
                    pToolBox->SetAlign( WINDOWALIGN_TOP );
                else
                    pToolBox->SetAlign( WINDOWALIGN_LEFT );
            }
            else
            {
                pToolBox->SetAlign( ImplConvertAlignment( aUIDockingElement.m_aDockedData.m_nDockedArea) );
            }
        }
    }
}

void SAL_CALL ToolbarLayoutManager::closed( const lang::EventObject& /*e*/ )
throw (uno::RuntimeException)
{
}

void SAL_CALL ToolbarLayoutManager::endPopupMode( const awt::EndPopupModeEvent& /*e*/ )
throw (uno::RuntimeException)
{
}

//---------------------------------------------------------------------------------------------------------
//  XUIConfigurationListener
//---------------------------------------------------------------------------------------------------------
void SAL_CALL ToolbarLayoutManager::elementInserted( const ui::ConfigurationEvent& rEvent )
throw (uno::RuntimeException)
{
    ::rtl::OUString aElementType;
    ::rtl::OUString aElementName;
    UIElement       aUIElement = implts_findToolbar( rEvent.ResourceURL );

    uno::Reference< ui::XUIElementSettings > xElementSettings( aUIElement.m_xUIElement, uno::UNO_QUERY );
    if ( xElementSettings.is() )
    {
        ::rtl::OUString aConfigSourcePropName( RTL_CONSTASCII_USTRINGPARAM( "ConfigurationSource" ));
        uno::Reference< beans::XPropertySet > xPropSet( xElementSettings, uno::UNO_QUERY );
        if ( xPropSet.is() )
        {
            if ( rEvent.Source == uno::Reference< uno::XInterface >( m_xDocCfgMgr, uno::UNO_QUERY ))
                xPropSet->setPropertyValue( aConfigSourcePropName, makeAny( m_xDocCfgMgr ));
        }
        xElementSettings->updateSettings();
    }
    else
    {
        parseResourceURL( rEvent.ResourceURL, aElementType, aElementName );
        if ( aElementName.indexOf( m_aCustomTbxPrefix ) != -1 )
        {
            // custom toolbar must be directly created, shown and layouted!
            createToolbar( rEvent.ResourceURL );
            uno::Reference< ui::XUIElement > xUIElement = getElement( rEvent.ResourceURL );
            if ( xUIElement.is() )
            {
                ::rtl::OUString                               aUIName;
                uno::Reference< ui::XUIConfigurationManager > xCfgMgr;
                uno::Reference< beans::XPropertySet >         xPropSet;

                try
                {
                    xCfgMgr  = uno::Reference< ui::XUIConfigurationManager >( rEvent.Source, uno::UNO_QUERY );
                    xPropSet = uno::Reference< beans::XPropertySet >( xCfgMgr->getSettings( rEvent.ResourceURL, sal_False ), uno::UNO_QUERY );

                    if ( xPropSet.is() )
                        xPropSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "UIName" ))) >>= aUIName;
                }
                catch ( container::NoSuchElementException& )
                {
                }
                catch ( beans::UnknownPropertyException& )
                {
                }
                catch ( lang::WrappedTargetException& )
                {
                }

                {
                    vos::OGuard aGuard( Application::GetSolarMutex() );
                    Window* pWindow = getWindowFromXUIElement( xUIElement );
                    if ( pWindow  )
                        pWindow->SetText( aUIName );
                }

                showToolbar( rEvent.ResourceURL );
            }
        }
    }
}

void SAL_CALL ToolbarLayoutManager::elementRemoved( const ui::ConfigurationEvent& rEvent )
throw (uno::RuntimeException)
{
    ::rtl::OUString aElementType;
    ::rtl::OUString aElementName;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    uno::Reference< awt::XWindow > xContainerWindow( m_xContainerWindow, uno::UNO_QUERY );
    uno::Reference< ui::XUIConfigurationManager > xModuleCfgMgr( m_xModuleCfgMgr );
    uno::Reference< ui::XUIConfigurationManager > xDocCfgMgr( m_xDocCfgMgr );
    aReadLock.unlock();
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */

    UIElement aUIElement = implts_findToolbar( rEvent.ResourceURL );
    uno::Reference< ui::XUIElementSettings > xElementSettings( aUIElement.m_xUIElement, uno::UNO_QUERY );
    if ( xElementSettings.is() )
    {
        bool                                  bNoSettings( false );
        ::rtl::OUString                       aConfigSourcePropName( RTL_CONSTASCII_USTRINGPARAM( "ConfigurationSource" ));
        uno::Reference< uno::XInterface >     xElementCfgMgr;
        uno::Reference< beans::XPropertySet > xPropSet( xElementSettings, uno::UNO_QUERY );

        if ( xPropSet.is() )
            xPropSet->getPropertyValue( aConfigSourcePropName ) >>= xElementCfgMgr;

        if ( !xElementCfgMgr.is() )
            return;

        // Check if the same UI configuration manager has changed => check further
        if ( rEvent.Source == xElementCfgMgr )
        {
            // Same UI configuration manager where our element has its settings
        if ( rEvent.Source == uno::Reference< uno::XInterface >( xDocCfgMgr, uno::UNO_QUERY ))
            {
                // document settings removed
                if ( xModuleCfgMgr->hasSettings( rEvent.ResourceURL ))
                {
                    xPropSet->setPropertyValue( aConfigSourcePropName, makeAny( xModuleCfgMgr ));
                    xElementSettings->updateSettings();
                    return;
                }
            }

            bNoSettings = true;
        }

        // No settings anymore, element must be destroyed
        if ( xContainerWindow.is() && bNoSettings )
        destroyToolbar( rEvent.ResourceURL );
    }
}

void SAL_CALL ToolbarLayoutManager::elementReplaced(
    const ui::ConfigurationEvent& /*Event*/ )
throw (uno::RuntimeException)
{
}

//---------------------------------------------------------------------------------------------------------
// XLayoutManager forwards
//---------------------------------------------------------------------------------------------------------
uno::Reference< ui::XUIElement > ToolbarLayoutManager::getElement( const ::rtl::OUString& aName )
{
    return implts_findToolbar( aName ).m_xUIElement;
}

uno::Sequence< uno::Reference< ui::XUIElement > > ToolbarLayoutManager::getElements()
{
    uno::Sequence< uno::Reference< ui::XUIElement > > aSeq;
    return aSeq;
}

::sal_Bool ToolbarLayoutManager::dockAllWindows( ::sal_Int16 /*nElementType*/ )
{
    return false;
}

sal_Bool ToolbarLayoutManager::floatWindow( const ::rtl::OUString& /*aName*/ )
{
    return false;
}

::sal_Bool ToolbarLayoutManager::lockWindow( const ::rtl::OUString& /*ResourceURL*/ )
{
    return false;
}

::sal_Bool ToolbarLayoutManager::unlockWindow( const ::rtl::OUString& /*ResourceURL*/ )
{
    return false;
}

void ToolbarLayoutManager::setElementSize( const ::rtl::OUString& /*aName*/, const awt::Size& /*aSize*/ )
{
}

void ToolbarLayoutManager::setElementPos( const ::rtl::OUString& /*aName*/, const awt::Point& /*aPos*/ )
{
}

void ToolbarLayoutManager::setElementPosSize( const ::rtl::OUString& /*aName*/, const awt::Point& /*aPos*/, const awt::Size& /*aSize*/ )
{
}

sal_Bool ToolbarLayoutManager::isElementVisible( const ::rtl::OUString& rResourceURL )
{
    UIElement aUIElement = implts_findToolbar( rResourceURL );
    if ( aUIElement.m_xUIElement.is() )
    {
        uno::Reference< css::awt::XWindow2 > xWindow2( aUIElement.m_xUIElement->getRealInterface(), uno::UNO_QUERY );

        if( xWindow2.is() )
            return xWindow2->isVisible();
    }

    return false;
}

sal_Bool ToolbarLayoutManager::isElementFloating( const ::rtl::OUString& rResourceURL )
{
    UIElement aUIElement = implts_findToolbar( rResourceURL );
    if ( aUIElement.m_xUIElement.is() )
    {
        if ( aUIElement.m_bFloating )
        {
            uno::Reference< css::awt::XWindow2 > xWindow2( aUIElement.m_xUIElement->getRealInterface(), uno::UNO_QUERY );

            if( xWindow2.is() )
                return true;
        }
    }

    return false;
}

sal_Bool ToolbarLayoutManager::isElementDocked( const ::rtl::OUString& rResourceURL )
{
    UIElement aUIElement = implts_findToolbar( rResourceURL );
    if ( aUIElement.m_xUIElement.is() )
    {
        if ( !aUIElement.m_bFloating )
        {
            uno::Reference< css::awt::XWindow2 > xWindow2( aUIElement.m_xUIElement->getRealInterface(), uno::UNO_QUERY );

            if( xWindow2.is() )
                return true;
        }
    }

    return false;
}

::sal_Bool ToolbarLayoutManager::isElementLocked( const ::rtl::OUString& rResourceURL )
{
    UIElement aUIElement = implts_findToolbar( rResourceURL );
    if ( aUIElement.m_xUIElement.is() )
    {
        uno::Reference< awt::XDockableWindow > xDockWindow( aUIElement.m_xUIElement->getRealInterface(), uno::UNO_QUERY );
        return xDockWindow.is() && !xDockWindow->isLocked();
    }

    return false;
}

awt::Size ToolbarLayoutManager::getElementSize( const ::rtl::OUString& rResourceURL )
{
    UIElement aUIElement = implts_findToolbar( rResourceURL );
    Window* pWindow = getWindowFromXUIElement( aUIElement.m_xUIElement );
    if ( pWindow )
    {
        ::Size    aSize = pWindow->GetSizePixel();
        awt::Size aWinSize;
        aWinSize.Width  = aSize.Width();
        aWinSize.Height = aSize.Height();
        return aWinSize;
    }

    return awt::Size();
}

awt::Point ToolbarLayoutManager::getElementPos( const ::rtl::OUString& rResourceURL )
{
    awt::Point aPos;
    UIElement  aUIElement = implts_findToolbar( rResourceURL );

    uno::Reference< awt::XWindow > xWindow( aUIElement.m_xUIElement->getRealInterface(), uno::UNO_QUERY );
    uno::Reference< awt::XDockableWindow > xDockWindow( xWindow, uno::UNO_QUERY );
    if ( xDockWindow.is() )
    {
        if ( aUIElement.m_bFloating )
        {
            awt::Rectangle aRect = xWindow->getPosSize();
            aPos.X = aRect.X;
            aPos.Y = aRect.Y;
        }
        else
        {
            ::Point aVirtualPos = aUIElement.m_aDockedData.m_aPos;
            aPos.X = aVirtualPos.X();
            aPos.Y = aVirtualPos.Y();
        }
    }

    return aPos;
}

} // namespace framework
