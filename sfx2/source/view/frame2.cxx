/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: topfrm.cxx,v $
 * $Revision: 1.97 $
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
#include "precompiled_sfx2.hxx"

#include "impframe.hxx"
#include "objshimp.hxx"
#include "sfxhelp.hxx"
#include "viewfac.hxx"
#include "workwin.hxx"

#include "sfx2/app.hxx"
#include "sfx2/bindings.hxx"
#include "sfx2/dispatch.hxx"
#include "sfx2/docfac.hxx"
#include "sfx2/docfile.hxx"
#include "sfx2/event.hxx"
#include "sfx2/objface.hxx"
#include "sfx2/request.hxx"

#include <com/sun/star/awt/XWindow2.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XFramesSupplier.hpp>
#include <com/sun/star/frame/XLayoutManager.hpp>

#include <comphelper/componentcontext.hxx>
#include <comphelper/namedvaluecollection.hxx>
#include <comphelper/processfactory.hxx>
#include <svtools/eitem.hxx>
#include <svtools/intitem.hxx>
#include <svtools/itemset.hxx>
#include <svtools/rectitem.hxx>
#include <svtools/stritem.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <tools/diagnose_ex.h>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::beans;
using ::com::sun::star::lang::XMultiServiceFactory;
using ::com::sun::star::lang::XComponent;
using ::com::sun::star::frame::XComponentLoader;

//------------------------------------------------------------------------

class SfxFrameWindow_Impl : public Window
{
public:
    SfxFrame*           pFrame;

    SfxFrameWindow_Impl( SfxFrame* pF, Window& i_rContainerWindow );
    ~SfxFrameWindow_Impl( );

    virtual void        DataChanged( const DataChangedEvent& rDCEvt );
    virtual void        StateChanged( StateChangedType nStateChange );
    virtual long        PreNotify( NotifyEvent& rNEvt );
    virtual long        Notify( NotifyEvent& rEvt );
    virtual void        Resize();
    virtual void        GetFocus();
    void                DoResize();
    DECL_LINK(          CloserHdl, void* );
};

SfxFrameWindow_Impl::SfxFrameWindow_Impl( SfxFrame* pF, Window& i_rContainerWindow )
        : Window( &i_rContainerWindow, WB_BORDER | WB_CLIPCHILDREN | WB_NODIALOGCONTROL | WB_3DLOOK )
        , pFrame( pF )
{
}

SfxFrameWindow_Impl::~SfxFrameWindow_Impl( )
{
}

void SfxFrameWindow_Impl::DataChanged( const DataChangedEvent& rDCEvt )
{
    Window::DataChanged( rDCEvt );
    SfxWorkWindow *pWorkWin = pFrame->GetWorkWindow_Impl();
    if ( pWorkWin )
        pWorkWin->DataChanged_Impl( rDCEvt );
}

long SfxFrameWindow_Impl::Notify( NotifyEvent& rNEvt )
{
    if ( pFrame->IsClosing_Impl() || !pFrame->GetFrameInterface().is() )
        return sal_False;

    SfxViewFrame* pView = pFrame->GetCurrentViewFrame();
    if ( !pView || !pView->GetObjectShell() )
        return Window::Notify( rNEvt );

    if ( rNEvt.GetType() == EVENT_GETFOCUS )
    {
        if ( pView->GetViewShell() && !pView->GetViewShell()->GetUIActiveIPClient_Impl() && !pFrame->IsInPlace() )
        {
            DBG_TRACE("SfxFrame: GotFocus");
            pView->MakeActive_Impl( FALSE );
        }

        // TODO/LATER: do we still need this code?
        Window* pWindow = rNEvt.GetWindow();
        ULONG nHelpId  = 0;
        while ( !nHelpId && pWindow )
        {
            nHelpId = pWindow->GetHelpId();
            pWindow = pWindow->GetParent();
        }

        if ( nHelpId )
            SfxHelp::OpenHelpAgent( pFrame, nHelpId );

        // if focus was on an external window, the clipboard content might have been changed
        pView->GetBindings().Invalidate( SID_PASTE );
        pView->GetBindings().Invalidate( SID_PASTE_SPECIAL );
        return sal_True;
    }
    else if( rNEvt.GetType() == EVENT_KEYINPUT )
    {
        if ( pView->GetViewShell()->KeyInput( *rNEvt.GetKeyEvent() ) )
            return TRUE;
    }
    else if ( rNEvt.GetType() == EVENT_EXECUTEDIALOG /*|| rNEvt.GetType() == EVENT_INPUTDISABLE*/ )
    {
        pView->SetModalMode( sal_True );
        return sal_True;
    }
    else if ( rNEvt.GetType() == EVENT_ENDEXECUTEDIALOG /*|| rNEvt.GetType() == EVENT_INPUTENABLE*/ )
    {
        //EnableInput( sal_True, sal_True );
        pView->SetModalMode( sal_False );
        return sal_True;
    }

    return Window::Notify( rNEvt );
}

long SfxFrameWindow_Impl::PreNotify( NotifyEvent& rNEvt )
{
    USHORT nType = rNEvt.GetType();
    if ( nType == EVENT_KEYINPUT || nType == EVENT_KEYUP )
    {
        SfxViewFrame* pView = pFrame->GetCurrentViewFrame();
        SfxViewShell* pShell = pView ? pView->GetViewShell() : NULL;
        if ( pShell && pShell->HasKeyListeners_Impl() && pShell->HandleNotifyEvent_Impl( rNEvt ) )
            return sal_True;
    }
    else if ( nType == EVENT_MOUSEBUTTONUP || nType == EVENT_MOUSEBUTTONDOWN )
    {
        Window* pWindow = rNEvt.GetWindow();
        SfxViewFrame* pView = pFrame->GetCurrentViewFrame();
        SfxViewShell* pShell = pView ? pView->GetViewShell() : NULL;
        if ( pShell )
            if ( pWindow == pShell->GetWindow() || pShell->GetWindow()->IsChild( pWindow ) )
                if ( pShell->HasMouseClickListeners_Impl() && pShell->HandleNotifyEvent_Impl( rNEvt ) )
                    return sal_True;
    }

    if ( nType == EVENT_MOUSEBUTTONDOWN )
    {
        Window* pWindow = rNEvt.GetWindow();
        const MouseEvent* pMEvent = rNEvt.GetMouseEvent();
        Point aPos = pWindow->OutputToScreenPixel( pMEvent->GetPosPixel() );
        SfxWorkWindow *pWorkWin = pFrame->GetWorkWindow_Impl();
        if ( pWorkWin )
            pWorkWin->EndAutoShow_Impl( aPos );
    }

    return Window::PreNotify( rNEvt );
}

void SfxFrameWindow_Impl::GetFocus()
{
    if ( pFrame && !pFrame->IsClosing_Impl() && pFrame->GetCurrentViewFrame() && pFrame->GetFrameInterface().is() )
        pFrame->GetCurrentViewFrame()->MakeActive_Impl( TRUE );
}

void SfxFrameWindow_Impl::Resize()
{
    if ( IsReallyVisible() || IsReallyShown() || GetOutputSizePixel().Width() )
        DoResize();
}

void SfxFrameWindow_Impl::StateChanged( StateChangedType nStateChange )
{
    if ( nStateChange == STATE_CHANGE_INITSHOW )
    {
        pFrame->pImp->bHidden = FALSE;
        if ( pFrame->IsInPlace() )
            // TODO/MBA: workaround for bug in LayoutManager: the final resize does not get through because the
            // LayoutManager works asynchronously and between resize and time execution the DockingAcceptor was exchanged so that
            // the resize event never is sent to the component
            SetSizePixel( GetParent()->GetOutputSizePixel() );

        DoResize();
        SfxViewFrame* pView = pFrame->GetCurrentViewFrame();
        if ( pView )
            pView->GetBindings().GetWorkWindow_Impl()->ShowChilds_Impl();
    }

    Window::StateChanged( nStateChange );
}

void SfxFrameWindow_Impl::DoResize()
{
    if ( !pFrame->pImp->bLockResize )
        pFrame->Resize();
}

SfxFrame* SfxFrame::CreateBlank()
{
    SfxFrame* pFrame = NULL;
    try
    {
        ::comphelper::ComponentContext aContext( ::comphelper::getProcessServiceFactory() );
        Reference < XFrame > xDesktop( aContext.createComponent( "com.sun.star.frame.Desktop" ), UNO_QUERY_THROW );
        Reference < XFrame > xFrame = xDesktop->findFrame( DEFINE_CONST_UNICODE("_blank"), 0 );
        pFrame = Create( xFrame );
    }
    catch( const Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }
    return pFrame;
}

SfxFrame* SfxFrame::Create( SfxObjectShell& rDoc, Window& rWindow, USHORT nViewId, bool bHidden )
{
    SfxFrame* pFrame = NULL;
    try
    {
        // create and initialize new top level frame for this window
        ::comphelper::ComponentContext aContext( ::comphelper::getProcessServiceFactory() );
        Reference < XFramesSupplier > xDesktop( aContext.createComponent( "com.sun.star.frame.Desktop" ), UNO_QUERY_THROW );
        Reference < XFrame > xFrame( aContext.createComponent( "com.sun.star.frame.Frame"), UNO_QUERY_THROW );

        Reference< awt::XWindow2 > xWin( VCLUnoHelper::GetInterface ( &rWindow ), uno::UNO_QUERY_THROW );
        xFrame->initialize( xWin.get() );
        xDesktop->getFrames()->append( xFrame );

        if ( xWin->isActive() )
            xFrame->activate();

        // create load arguments
        Sequence< PropertyValue > aLoadArgs;
        TransformItems( SID_OPENDOC, *rDoc.GetMedium()->GetItemSet(), aLoadArgs );

        ::comphelper::NamedValueCollection aArgs( aLoadArgs );
        aArgs.put( "Model", rDoc.GetModel() );
        aArgs.put( "Hidden", bHidden );
        if ( nViewId )
            aArgs.put( "ViewId", nViewId );

        aLoadArgs = aArgs.getPropertyValues();

        // load the doc into that frame
        ::rtl::OUString sLoaderURL( rDoc.GetModel()->getURL() );
        if ( sLoaderURL.getLength() == 0 )
            sLoaderURL = rDoc.GetFactory().GetFactoryURL();

        Reference< XComponentLoader > xLoader( xFrame, UNO_QUERY_THROW );
        xLoader->loadComponentFromURL(
            sLoaderURL,
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "_self" ) ),
            0,
            aLoadArgs
        );

        for (   pFrame = SfxFrame::GetFirst();
                pFrame;
                pFrame = SfxFrame::GetNext( *pFrame )
            )
        {
            if ( pFrame->GetFrameInterface() == xFrame )
                break;
        }

        OSL_ENSURE( pFrame, "SfxFrame::Create: load succeeded, but no SfxFrame was created during this!" );
    }
    catch( const Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }

    return pFrame;
}

SfxFrame* SfxFrame::Create( Reference < XFrame > xFrame )
{
    // create a new TopFrame to an external XFrame object ( wrap controller )
    ENSURE_OR_THROW( xFrame.is(), "NULL frame not allowed" );
    Window* pWindow = VCLUnoHelper::GetWindow( xFrame->getContainerWindow() );
    ENSURE_OR_THROW( pWindow, "frame without container window not allowed" );

    SfxFrame* pFrame = new SfxFrame( *pWindow, false );
    pFrame->SetFrameInterface_Impl( xFrame );
    return pFrame;
}

SfxFrame::SfxFrame( Window& i_rContainerWindow, bool i_bHidden )
    :pParentFrame( NULL )
    ,pChildArr( NULL )
    ,pImp( NULL )
    ,pWindow( NULL )
{
    Construct_Impl();

    pImp->bHidden = i_bHidden;
    InsertTopFrame_Impl( this );
    pImp->pExternalContainerWindow = &i_rContainerWindow;

    pWindow = new SfxFrameWindow_Impl( this, i_rContainerWindow );

    // always show pWindow, which is the ComponentWindow of the XFrame we live in
    // nowadays, since SfxFrames can be created with an XFrame only, hiding or showing the complete XFrame
    // is not done at level of the container window, not at SFX level. Thus, the component window can
    // always be visible.
    pWindow->Show();
}

void SfxFrame::SetPresentationMode( BOOL bSet )
{
    if ( GetCurrentViewFrame() )
        GetCurrentViewFrame()->GetWindow().SetBorderStyle( bSet ? WINDOW_BORDER_NOBORDER : WINDOW_BORDER_NORMAL );

    Reference< com::sun::star::beans::XPropertySet > xPropSet( GetFrameInterface(), UNO_QUERY );
    Reference< ::com::sun::star::frame::XLayoutManager > xLayoutManager;

    if ( xPropSet.is() )
    {
        Any aValue = xPropSet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "LayoutManager" )));
        aValue >>= xLayoutManager;
    }

    if ( xLayoutManager.is() )
        xLayoutManager->setVisible( !bSet ); // we don't want to have ui in presentation mode

    SetMenuBarOn_Impl( !bSet );
    if ( GetWorkWindow_Impl() )
        GetWorkWindow_Impl()->SetDockingAllowed( !bSet );
    if ( GetCurrentViewFrame() )
        GetCurrentViewFrame()->GetDispatcher()->Update_Impl( TRUE );
}

SystemWindow* SfxFrame::GetSystemWindow() const
{
    return GetTopWindow_Impl();
}

SystemWindow* SfxFrame::GetTopWindow_Impl() const
{
    if ( pImp->pExternalContainerWindow->IsSystemWindow() )
        return (SystemWindow*) pImp->pExternalContainerWindow;
    else
        return NULL;
}

Window& SfxFrame::GetWindow() const
{
    return *pWindow;
}

sal_Bool SfxFrame::Close()
{
    delete this;
    return sal_True;
}

void SfxFrame::LockResize_Impl( BOOL bLock )
{
    pImp->bLockResize = bLock;
}

IMPL_LINK( SfxFrameWindow_Impl, CloserHdl, void*, EMPTYARG )
{
    if ( pFrame && !pFrame->PrepareClose_Impl( TRUE ) )
        return 0L;

    if ( pFrame )
        pFrame->GetCurrentViewFrame()->GetBindings().Execute( SID_CLOSEWIN, 0, 0, SFX_CALLMODE_ASYNCHRON );
    return 0L;
}

void SfxFrame::SetMenuBarOn_Impl( BOOL bOn )
{
    pImp->bMenuBarOn = bOn;

    Reference< com::sun::star::beans::XPropertySet > xPropSet( GetFrameInterface(), UNO_QUERY );
    Reference< ::com::sun::star::frame::XLayoutManager > xLayoutManager;

    if ( xPropSet.is() )
    {
        Any aValue = xPropSet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "LayoutManager" )));
        aValue >>= xLayoutManager;
    }

    if ( xLayoutManager.is() )
    {
        rtl::OUString aMenuBarURL( RTL_CONSTASCII_USTRINGPARAM( "private:resource/menubar/menubar" ));

        if ( bOn )
            xLayoutManager->showElement( aMenuBarURL );
        else
            xLayoutManager->hideElement( aMenuBarURL );
    }
}

BOOL SfxFrame::IsMenuBarOn_Impl() const
{
    return pImp->bMenuBarOn;
}

void SfxFrame::PositionWindow_Impl( const Rectangle& rWinArea ) const
{
    Window *pWin = pImp->pExternalContainerWindow;

    // Groesse setzen
    const Size aAppWindow( pImp->pExternalContainerWindow->GetDesktopRectPixel().GetSize() );
    Point aPos( rWinArea.TopLeft() );
    Size aSz( rWinArea.GetSize() );
    if ( aSz.Width() && aSz.Height() )
    {
        aPos.X() = Min(aPos.X(),
                        long(aAppWindow.Width() - aSz.Width() + aSz.Width() / 2) );
        aPos.Y() = Min(aPos.Y(),
                        long( aAppWindow.Height() - aSz.Height() + aSz.Height() / 2) );
        if ( aPos.X() + aSz.Width() <
                aAppWindow.Width() + aSz.Width() / 2 &&
                aPos.Y() + aSz.Height() <
                aAppWindow.Height() + aSz.Height() / 2 )
        {
            pWin->SetPosPixel( aPos );
            pWin->SetOutputSizePixel( aSz );
        }
    }
}

sal_Bool SfxFrame::InsertDocument_Impl( SfxObjectShell& rDoc, const ::comphelper::NamedValueCollection& i_rArgs )
/* [Beschreibung]
 */
{
    OSL_PRECOND( rDoc.GetMedium(), "SfxFrame::InsertDocument_Impl: no medium -> no view!");
    if ( !rDoc.GetMedium() )
        return sal_False;

    OSL_PRECOND( GetCurrentViewFrame() == NULL,
        "SfxObjectShell::InsertDocument_Impl: no support (anymore) for loading into a non-empty frame!" );
        // Since some refactoring in CWS autorecovery, this shouldn't happen anymore. Frame re-usage is nowadays
        // done in higher layers, namely in the framework.
    if ( GetCurrentViewFrame() != NULL )
        return sal_False;

    OSL_PRECOND( GetCurrentDocument() == NULL,
        "SfxFrame::InsertDocument_Impl: re-using an Sfx(Top)Frame is not supported anymore!" );

    // view ID
    sal_Int16 nViewId = i_rArgs.getOrDefault( "ViewId", sal_Int16( 0 ) );

    // jump mark
    const bool bHasJumpMark = i_rArgs.has( "JumpMark" );

    // plugin mode
    sal_Int16 nPluginMode = 0;
    const bool bHasPluginMode = i_rArgs.get_ensureType( "PluginMode", nPluginMode );

    // hidden?
    pImp->bHidden = i_rArgs.getOrDefault( "Hidden", pImp->bHidden );

    UpdateDescriptor( &rDoc );

    if ( nPluginMode && ( nPluginMode != 2 ) )
        SetInPlace_Impl( TRUE );

    SfxViewFrame* pViewFrame = SfxViewFrame::Create_Impl( *this, rDoc, nViewId ? nViewId : rDoc.GetFactory().GetViewFactory( 0 ).GetOrdinal() );
    OSL_ENSURE( pViewFrame, "SfxFrame::InsertDocument_Impl: something went wrong while creating the SfxViewFrame!" );
    if ( !pViewFrame )
        // TODO: better error handling? Under which conditions can this fail?
        return sal_False;

    return GetCurrentDocument() == &rDoc;
}

bool SfxFrame::IsMarkedHidden_Impl() const
{
    return pImp->bHidden;
}
