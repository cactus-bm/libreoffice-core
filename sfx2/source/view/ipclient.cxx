/*************************************************************************
 *
 *  $RCSfile: ipclient.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-01-31 08:45:49 $
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

#ifndef _COM_SUN_STAR_EMBED_EMBEDSTATES_HPP_
#include <com/sun/star/embed/EmbedStates.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XVISUALOBJECT_HPP_
#include <com/sun/star/embed/XVisualObject.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XEMBEDDEDCLIENT_HPP_
#include <com/sun/star/embed/XEmbeddedClient.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XINPLACECLIENT_HPP_
#include <com/sun/star/embed/XInplaceClient.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XINPLACEOBJECT_HPP_
#include <com/sun/star/embed/XInplaceObject.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XCOMPONENTSUPPLIER_HPP_
#include <com/sun/star/embed/XComponentSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XWINDOWSUPPLIER_HPP_
#include <com/sun/star/embed/XWindowSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XEMBEDPERSIST_HPP_
#include <com/sun/star/embed/XEmbedPersist.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCHILD_HPP_
#include <com/sun/star/container/XChild.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XSTATECHANGELISTENER_HPP_
#include <com/sun/star/embed/XStateChangeListener.hpp>
#endif

#include <svtools/embedhlp.hxx>

#include "ipclient.hxx"
#include "viewsh.hxx"
#include "viewfrm.hxx"
#include "objsh.hxx"
#include "dispatch.hxx"
#include "workwin.hxx"
#include "guisaveas.hxx"

#ifndef _CPPUHELPER_IMPLBASE5_HXX_
#include <cppuhelper/implbase5.hxx>
#endif
#ifndef _SV_SALBTYPE_HXX
#include <vcl/salbtype.hxx>
#endif
#ifndef _EHDL_HXX
#include <svtools/ehdl.hxx>
#endif

#include <vcl/timer.hxx>
#include <vcl/window.hxx>
#include <toolkit/awt/vclxwindow.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <toolkit/helper/convert.hxx>
#include <tools/fract.hxx>
#include <tools/gen.hxx>
#include <svtools/rectitem.hxx>
#include <svtools/soerr.hxx>


#define SFX_CLIENTACTIVATE_TIMEOUT 100

using namespace com::sun::star;

//====================================================================
// SfxInPlaceClient_Impl

//--------------------------------------------------------------------
class SfxInPlaceClient_Impl : public ::cppu::WeakImplHelper5< embed::XEmbeddedClient,
                                                              embed::XInplaceClient,
                                                              document::XEventListener,
                                                              embed::XStateChangeListener,
                                                              embed::XWindowSupplier >
{
public:
    Timer                           m_aTimer;               // activation timeout, starts after object connection
    Rectangle                       m_aObjArea;             // area of object in coordinate system of the container (without scaling)
    Fraction                        m_aScaleWidth;          // scaling that was applied to the object when it was not active
    Fraction                        m_aScaleHeight;
    SfxInPlaceClient*               m_pClient;
    sal_Int64                       m_nAspect;              // ViewAspect that is assigned from the container
    Rectangle                       m_aLastObjAreaPixel;    // area of object in coordinate system of the container (without scaling)
    sal_Bool                        m_bStoreObject;

    uno::Reference < embed::XEmbeddedObject > m_xObject;
    uno::Reference < embed::XEmbeddedClient > m_xClient;

    void SizeHasChanged();
    DECL_LINK           (TimerHdl, Timer*);
    uno::Reference < frame::XFrame > GetFrame() const;

    // XEmbeddedClient
    virtual void SAL_CALL saveObject() throw ( embed::ObjectSaveVetoException, uno::Exception, uno::RuntimeException );
    virtual void SAL_CALL visibilityChanged( sal_Bool bVisible ) throw ( embed::WrongStateException, uno::RuntimeException );

    // XInplaceClient
    virtual sal_Bool SAL_CALL canInplaceActivate() throw ( uno::RuntimeException );
    virtual void SAL_CALL activatingInplace() throw ( embed::WrongStateException, uno::RuntimeException );
    virtual void SAL_CALL activatingUI() throw ( embed::WrongStateException, uno::RuntimeException );
    virtual void SAL_CALL deactivatedInplace() throw ( embed::WrongStateException, uno::RuntimeException );
    virtual void SAL_CALL deactivatedUI() throw ( embed::WrongStateException, uno::RuntimeException );
    virtual uno::Reference< ::drafts::com::sun::star::frame::XLayoutManager > SAL_CALL getLayoutManager() throw ( embed::WrongStateException, uno::RuntimeException );
    virtual uno::Reference< frame::XDispatchProvider > SAL_CALL getInplaceDispatchProvider() throw ( embed::WrongStateException, uno::RuntimeException );
    virtual awt::Rectangle SAL_CALL getPlacement() throw ( embed::WrongStateException, uno::RuntimeException );
    virtual awt::Rectangle SAL_CALL getClipRectangle() throw ( embed::WrongStateException, uno::RuntimeException );
    virtual void SAL_CALL translateAccelerators( const uno::Sequence< awt::KeyEvent >& aKeys ) throw ( embed::WrongStateException, uno::RuntimeException );
    virtual void SAL_CALL scrollObject( const awt::Size& aOffset ) throw ( embed::WrongStateException, uno::RuntimeException );
    virtual void SAL_CALL changedPlacement( const awt::Rectangle& aPosRect ) throw ( embed::WrongStateException, uno::Exception, uno::RuntimeException );

    // XComponentSupplier
    virtual uno::Reference< util::XCloseable > SAL_CALL getComponent() throw ( uno::RuntimeException );

    // XWindowSupplier
    virtual uno::Reference< awt::XWindow > SAL_CALL getWindow() throw ( uno::RuntimeException );

    // document::XEventListener
    virtual void SAL_CALL       notifyEvent( const document::EventObject& aEvent ) throw( uno::RuntimeException );

    // XStateChangeListener
    virtual void SAL_CALL changingState( const ::com::sun::star::lang::EventObject& aEvent, ::sal_Int32 nOldState, ::sal_Int32 nNewState ) throw (::com::sun::star::embed::WrongStateException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL stateChanged( const ::com::sun::star::lang::EventObject& aEvent, ::sal_Int32 nOldState, ::sal_Int32 nNewState ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& aEvent ) throw (::com::sun::star::uno::RuntimeException);
};

void SAL_CALL SfxInPlaceClient_Impl::changingState( const ::com::sun::star::lang::EventObject& aEvent, ::sal_Int32 nOldState, ::sal_Int32 nNewState ) throw (::com::sun::star::embed::WrongStateException, ::com::sun::star::uno::RuntimeException)
{
}

void SAL_CALL SfxInPlaceClient_Impl::stateChanged( const ::com::sun::star::lang::EventObject& aEvent, ::sal_Int32 nOldState, ::sal_Int32 nNewState ) throw (::com::sun::star::uno::RuntimeException)
{
    if ( m_pClient && nOldState != embed::EmbedStates::LOADED && nNewState == embed::EmbedStates::RUNNING )
    {
        // deactivation of object
        SfxObjectShell::SetWorkingDocument( m_pClient->GetViewShell()->GetObjectShell() );
    }
}

void SAL_CALL SfxInPlaceClient_Impl::notifyEvent( const document::EventObject& aEvent ) throw( uno::RuntimeException )
{
    DBG_ASSERT( m_pClient, "No Client, but notified");
    if ( m_pClient && aEvent.EventName.equalsAscii("OnVisAreaChanged") )
    {
        m_pClient->ViewChanged();
    }
}

void SAL_CALL SfxInPlaceClient_Impl::disposing( const ::com::sun::star::lang::EventObject& aEvent ) throw (::com::sun::star::uno::RuntimeException)
{
    DELETEZ( m_pClient );
}

// XEmbeddedClient
//--------------------------------------------------------------------
uno::Reference < frame::XFrame > SfxInPlaceClient_Impl::GetFrame() const
{
    if ( !m_pClient )
        throw uno::RuntimeException();
    return m_pClient->GetViewShell()->GetViewFrame()->GetFrame()->GetFrameInterface();
}

void SAL_CALL SfxInPlaceClient_Impl::saveObject()
    throw ( embed::ObjectSaveVetoException,
            uno::Exception,
            uno::RuntimeException )
{
    if ( !m_bStoreObject )
        // client wants to discard the object (usually it means the container document is closed while an object is active
        // and the user didn't request saving the changes
        return;

    // the common persistance is supported by objects and links
    uno::Reference< embed::XCommonEmbedPersist > xPersist( m_xObject, uno::UNO_QUERY );
    if ( !xPersist.is() )
        throw uno::RuntimeException();

    try
    {
        xPersist->storeOwn();
        m_xObject->update();
    }
    catch ( uno::Exception& )
    {
        //TODO/LATER: what should happen if object can't be saved?!
    }

    // the client can exist only in case there is a view shell
    if ( !m_pClient || !m_pClient->GetViewShell() )
        throw uno::RuntimeException();

    SfxObjectShell* pDocShell = m_pClient->GetViewShell()->GetObjectShell();
    if ( !pDocShell )
        throw uno::RuntimeException();

    pDocShell->SetModified( sal_True );

    //TODO/LATER: invalidation might be necessary when object was modified, but is not
    //saved through this method
    // m_pClient->Invalidate();
}

//--------------------------------------------------------------------
void SAL_CALL SfxInPlaceClient_Impl::visibilityChanged( sal_Bool bVisible )
    throw ( embed::WrongStateException,
            uno::RuntimeException )
{
    if ( !m_pClient || !m_pClient->GetViewShell() )
        throw uno::RuntimeException();

    m_pClient->GetViewShell()->OutplaceActivated( bVisible, m_pClient );
}


// XInplaceClient
//--------------------------------------------------------------------
sal_Bool SAL_CALL SfxInPlaceClient_Impl::canInplaceActivate()
    throw ( uno::RuntimeException )
{
    if ( !m_xObject.is() )
        throw uno::RuntimeException();

    // we don't want to switch directly from outplace to inplace mode
    if ( m_xObject->getCurrentState() == embed::EmbedStates::ACTIVE )
        return sal_False;

    return sal_True;
}

//--------------------------------------------------------------------
void SAL_CALL SfxInPlaceClient_Impl::activatingInplace()
    throw ( embed::WrongStateException,
            uno::RuntimeException )
{
    if ( !m_pClient || !m_pClient->GetViewShell() )
        throw uno::RuntimeException();

    uno::Reference < container::XChild > xChild( m_xObject->getComponent(), uno::UNO_QUERY );
    if ( xChild.is() && !xChild->getParent().is() )
        xChild->setParent( m_pClient->GetViewShell()->GetObjectShell()->GetModel() );

    m_pClient->GetViewShell()->InplaceActivating( m_pClient );
}

//--------------------------------------------------------------------
void SAL_CALL SfxInPlaceClient_Impl::activatingUI()
    throw ( embed::WrongStateException,
            uno::RuntimeException )
{
    if ( !m_pClient || !m_pClient->GetViewShell() )
        throw uno::RuntimeException();

#if 0
    // make LayoutManager of container invisible (currently needs SFX code)
    uno::Reference < drafts::com::sun::star::frame::XLayoutManager > xLayoutManager = getLayoutManager();
    xLayoutManager->lock();
    xLayoutManager->setVisible( sal_False );
#endif
    SfxWorkWindow *pWorkWin = m_pClient->GetViewShell()->GetViewFrame()->GetFrame()->GetWorkWindow_Impl();
    pWorkWin->Lock_Impl( TRUE );
    pWorkWin->MakeVisible_Impl( FALSE );

    // make LayoutManager of object visible and force placement of objects tools
    uno::Reference < lang::XUnoTunnel > xObj( m_xObject->getComponent(), uno::UNO_QUERY );
    uno::Sequence < sal_Int8 > aSeq( SvGlobalName( SFX_GLOBAL_CLASSID ).GetByteSequence() );
    sal_Int64 nHandle = xObj.is() ? xObj->getSomething( aSeq ) : 0;
    if ( nHandle )
    {
        // currently needs SFX code
        SfxObjectShell* pDoc = (SfxObjectShell*) (sal_Int32*) nHandle;
        SfxViewFrame* pFrame = SfxViewFrame::GetFirst( pDoc );
        SfxWorkWindow *pWorkWin = pFrame->GetFrame()->GetWorkWindow_Impl();
        pWorkWin->MakeVisible_Impl( TRUE );
        pWorkWin->Lock_Impl( FALSE );

#if 0
        uno::Reference < beans::XPropertySet > xFrame( pFrame->GetFrame()->GetFrameInterface(), uno::UNO_QUERY );
        try
        {
            uno::Reference< ::drafts::com::sun::star::frame::XLayoutManager > xMan;
            uno::Any aAny = xFrame->getPropertyValue( ::rtl::OUString::createFromAscii("LayoutManager") );
            aAny >>= xMan;
            xMan->setVisible( sal_True );
            xMan->unlock();
        }
        catch ( uno::Exception& )
        {
        }
#endif
        pDoc->UIActivate( TRUE );
    }

    // remove containers tools
    m_pClient->GetViewShell()->UIActivating( m_pClient );
}

//--------------------------------------------------------------------
void SAL_CALL SfxInPlaceClient_Impl::deactivatedInplace()
    throw ( embed::WrongStateException,
            uno::RuntimeException )
{
    if ( !m_pClient || !m_pClient->GetViewShell() )
        throw uno::RuntimeException();

    m_pClient->GetViewShell()->InplaceDeactivated( m_pClient );
    uno::Reference < container::XChild > xChild( m_xObject->getComponent(), uno::UNO_QUERY );
    if ( xChild.is() && xChild->getParent().is() )
        xChild->setParent( uno::Reference < uno::XInterface >() );
}

//--------------------------------------------------------------------
void SAL_CALL SfxInPlaceClient_Impl::deactivatedUI()
    throw ( embed::WrongStateException,
            uno::RuntimeException )
{
    if ( !m_pClient || !m_pClient->GetViewShell() )
        throw uno::RuntimeException();

#if 0
    // make LayoutManager of container visible again (currently needs SFX code)
    uno::Reference < drafts::com::sun::star::frame::XLayoutManager > xLayoutManager = getLayoutManager();
    xLayoutManager->setVisible( sal_True );
    xLayoutManager->unlock();
#endif

    SfxWorkWindow *pWorkWin = m_pClient->GetViewShell()->GetViewFrame()->GetFrame()->GetWorkWindow_Impl();
    pWorkWin->MakeVisible_Impl( TRUE );
    pWorkWin->Lock_Impl( FALSE );

    // make LayoutManager of object invisible
    SfxObjectShell* pDoc = 0;
    uno::Reference < lang::XUnoTunnel > xObj( m_xObject->getComponent(), uno::UNO_QUERY );
    uno::Sequence < sal_Int8 > aSeq( SvGlobalName( SFX_GLOBAL_CLASSID ).GetByteSequence() );
    sal_Int64 nHandle = xObj.is() ? xObj->getSomething( aSeq ) : 0;
    if ( nHandle )
    {
        // currently needs SFX code
        pDoc = (SfxObjectShell*) (sal_Int32*) nHandle;
        SfxViewFrame* pFrame = SfxViewFrame::GetFirst( pDoc );
        SfxWorkWindow *pWorkWin = pFrame->GetFrame()->GetWorkWindow_Impl();
        pWorkWin->MakeVisible_Impl( FALSE );
        pWorkWin->Lock_Impl( TRUE );

#if 0
        uno::Reference < beans::XPropertySet > xFrame( pFrame->GetFrame()->GetFrameInterface(), uno::UNO_QUERY );
        try
        {
            uno::Reference< ::drafts::com::sun::star::frame::XLayoutManager > xMan;
            uno::Any aAny = xFrame->getPropertyValue( ::rtl::OUString::createFromAscii("LayoutManager") );
            aAny >>= xMan;
            xMan->setVisible( sal_False );
            xMan->lock();
        }
        catch ( uno::Exception& )
        {
        }
#endif
    }

    // show containers tools
    m_pClient->GetViewShell()->UIDeactivated( m_pClient );

    if ( pDoc )
        // force removal of objects tools
        pDoc->UIActivate( FALSE );
}

//--------------------------------------------------------------------
uno::Reference< ::drafts::com::sun::star::frame::XLayoutManager > SAL_CALL SfxInPlaceClient_Impl::getLayoutManager()
    throw ( embed::WrongStateException,
            uno::RuntimeException )
{
    uno::Reference < beans::XPropertySet > xFrame( GetFrame(), uno::UNO_QUERY );
    if ( !xFrame.is() )
        throw uno::RuntimeException();

    uno::Reference< ::drafts::com::sun::star::frame::XLayoutManager > xMan;
    try
    {
        uno::Any aAny = xFrame->getPropertyValue( ::rtl::OUString::createFromAscii("LayoutManager") );
        aAny >>= xMan;
    }
    catch ( uno::Exception& )
    {
        throw uno::RuntimeException();
    }

    return xMan;
}

//--------------------------------------------------------------------
uno::Reference< frame::XDispatchProvider > SAL_CALL SfxInPlaceClient_Impl::getInplaceDispatchProvider()
    throw ( embed::WrongStateException,
            uno::RuntimeException )
{
    return uno::Reference < frame::XDispatchProvider >( GetFrame(), uno::UNO_QUERY_THROW );
}

//--------------------------------------------------------------------
awt::Rectangle SAL_CALL SfxInPlaceClient_Impl::getPlacement()
    throw ( embed::WrongStateException,
            uno::RuntimeException )
{
    if ( !m_pClient || !m_pClient->GetViewShell() )
        throw uno::RuntimeException();

    // apply scaling to object area and convert to pixels
    Rectangle aRealObjArea( m_aObjArea );
    aRealObjArea.SetSize( Size( Fraction( aRealObjArea.GetWidth() ) * m_aScaleWidth,
                                Fraction( aRealObjArea.GetHeight() ) * m_aScaleHeight ) );

    aRealObjArea = m_pClient->GetEditWin()->LogicToPixel( aRealObjArea );
    return AWTRectangle( aRealObjArea );
}

//--------------------------------------------------------------------
awt::Rectangle SAL_CALL SfxInPlaceClient_Impl::getClipRectangle()
    throw ( embed::WrongStateException,
            uno::RuntimeException )
{
    if ( !m_pClient || !m_pClient->GetViewShell() )
        throw uno::RuntimeException();

    // currently(?) same as placement
    Rectangle aRealObjArea( m_aObjArea );
    aRealObjArea.SetSize( Size( Fraction( aRealObjArea.GetWidth() ) * m_aScaleWidth,
                                Fraction( aRealObjArea.GetHeight() ) * m_aScaleHeight ) );

    aRealObjArea = m_pClient->GetEditWin()->LogicToPixel( aRealObjArea );
    return AWTRectangle( aRealObjArea );
}

//--------------------------------------------------------------------
void SAL_CALL SfxInPlaceClient_Impl::translateAccelerators( const uno::Sequence< awt::KeyEvent >& aKeys )
    throw ( embed::WrongStateException,
            uno::RuntimeException )
{
    if ( !m_pClient || !m_pClient->GetViewShell() )
        throw uno::RuntimeException();

    // TODO/MBA: keyboard accelerators
}

//--------------------------------------------------------------------
void SAL_CALL SfxInPlaceClient_Impl::scrollObject( const awt::Size& aOffset )
    throw ( embed::WrongStateException,
            uno::RuntimeException )
{
    if ( !m_pClient || !m_pClient->GetViewShell() )
        throw uno::RuntimeException();
}

//--------------------------------------------------------------------
void SAL_CALL SfxInPlaceClient_Impl::changedPlacement( const awt::Rectangle& aPosRect )
    throw ( embed::WrongStateException,
            uno::Exception,
            uno::RuntimeException )
{
    uno::Reference< embed::XInplaceObject > xInplace( m_xObject, uno::UNO_QUERY );
    if ( !xInplace.is() || !m_pClient || !m_pClient->GetEditWin() || !m_pClient->GetViewShell() )
        throw uno::RuntimeException();

    // check if the change is at least one pixel in size
    awt::Rectangle aOldRect = getPlacement();
    Rectangle aNewPixelRect = VCLRectangle( aPosRect );
    Rectangle aOldPixelRect = VCLRectangle( aOldRect );
    if ( aOldPixelRect == aNewPixelRect )
        // nothing has changed
        return;

    // new scaled object area
    Rectangle aNewLogicRect = m_pClient->GetEditWin()->PixelToLogic( aNewPixelRect );

    // allow container to apply restrictions on the requested new area
    m_pClient->RequestNewObjectArea( aNewLogicRect);

    // new size of the object area without scaling
    Size aNewObjSize( Fraction( aNewLogicRect.GetWidth() ) / m_aScaleWidth,
                      Fraction( aNewLogicRect.GetHeight() ) / m_aScaleHeight );

    if ( aNewLogicRect.GetSize() != m_pClient->GetScaledObjArea().GetSize() )
    {
        // size has changed, so first change visual area of the object before we resize its view
        // without this the object always would be scaled - now it has the choice
        MapMode aObjectMap( VCLUnoHelper::UnoEmbed2VCLMapUnit( m_xObject->getMapUnit( m_nAspect ) ) );
        MapMode aClientMap( m_pClient->GetEditWin()->GetMapMode().GetMapUnit() );

        // convert to logical coordinates of the embedded object
        Size aNewSize = m_pClient->GetEditWin()->LogicToLogic( aNewObjSize, &aClientMap, &aObjectMap );
        m_xObject->setVisualAreaSize( m_nAspect, awt::Size( aNewSize.Width(), aNewSize.Height() ) );
    }

    // resize object view
    aNewPixelRect = m_pClient->GetEditWin()->LogicToPixel( aNewLogicRect );
    awt::Rectangle aR = AWTRectangle( aNewPixelRect );
    xInplace->setObjectRectangles( aR, aR );

    // now remove scaling from new placement and keep this a the new object area
    aNewLogicRect.SetSize( aNewObjSize );
    m_aObjArea = aNewLogicRect;

    // notify container view about changes
    m_pClient->ObjectAreaChanged();
}

// XComponentSupplier
//--------------------------------------------------------------------
uno::Reference< util::XCloseable > SAL_CALL SfxInPlaceClient_Impl::getComponent()
    throw ( uno::RuntimeException )
{
    if ( !m_pClient || !m_pClient->GetViewShell() )
        throw uno::RuntimeException();

    SfxObjectShell* pDocShell = m_pClient->GetViewShell()->GetObjectShell();
    if ( !pDocShell )
        throw uno::RuntimeException();

    // all the components must implement XCloseable
    uno::Reference< util::XCloseable > xComp( pDocShell->GetModel(), uno::UNO_QUERY );
    if ( !xComp.is() )
        throw uno::RuntimeException();

    return xComp;
}


// XWindowSupplier
//--------------------------------------------------------------------
uno::Reference< awt::XWindow > SAL_CALL SfxInPlaceClient_Impl::getWindow()
    throw ( uno::RuntimeException )
{
    if ( !m_pClient || !m_pClient->GetEditWin() )
        throw uno::RuntimeException();

    uno::Reference< awt::XWindow > xWin( m_pClient->GetEditWin()->GetComponentInterface(), uno::UNO_QUERY );
    return xWin;
}

//--------------------------------------------------------------------
// notification to the client implementation that either the object area or the scaling has been changed
// as a result the logical size of the window has changed also
void SfxInPlaceClient_Impl::SizeHasChanged()
{
    if ( !m_pClient || !m_pClient->GetViewShell() )
        throw uno::RuntimeException();

    try {
        if ( m_xObject.is()
          && ( m_xObject->getCurrentState() == embed::EmbedStates::INPLACE_ACTIVE
                || m_xObject->getCurrentState() == embed::EmbedStates::UI_ACTIVE ) )
        {
            // only possible in active states
            uno::Reference< embed::XInplaceObject > xInplace( m_xObject, uno::UNO_QUERY );
            if ( !xInplace.is() )
                throw uno::RuntimeException();

            // convert new size to pixels
            Rectangle aRealObjArea( m_aObjArea );
            aRealObjArea.SetSize( Size( Fraction( aRealObjArea.GetWidth() ) * m_aScaleWidth,
                                        Fraction( aRealObjArea.GetHeight() ) * m_aScaleHeight ) );
            aRealObjArea = m_pClient->GetEditWin()->LogicToPixel( aRealObjArea );

            // notify object about the new size so that the view size will be changed also
            awt::Rectangle aAwtRect = AWTRectangle( aRealObjArea );

            xInplace->setObjectRectangles( aAwtRect, aAwtRect );
        }
    }
    catch( uno::Exception& )
    {
        // TODO/LATER: handle error
    }
}

//--------------------------------------------------------------------
IMPL_LINK( SfxInPlaceClient_Impl, TimerHdl, Timer*, pTimer )
{
    if ( m_pClient && m_xObject.is() )
        m_pClient->GetViewShell()->CheckIPClient_Impl( m_pClient, m_pClient->GetViewShell()->GetObjectShell()->GetVisArea() );
    return 0;
}


//====================================================================
// SfxInPlaceClient

//--------------------------------------------------------------------
SfxInPlaceClient::SfxInPlaceClient( SfxViewShell* pViewShell, Window *pDraw, sal_Int64 nAspect ) :
    m_pImp( new SfxInPlaceClient_Impl ),
    m_pViewSh( pViewShell ),
    m_pEditWin( pDraw )
{
    m_pImp->m_pClient = this;
    m_pImp->m_nAspect = nAspect;
    m_pImp->m_aScaleWidth = m_pImp->m_aScaleHeight = Fraction(1,1);
    m_pImp->m_xClient = static_cast< embed::XEmbeddedClient* >( m_pImp );
    pViewShell->NewIPClient_Impl(this);
    m_pImp->m_aTimer.SetTimeout( SFX_CLIENTACTIVATE_TIMEOUT );
    m_pImp->m_aTimer.SetTimeoutHdl( LINK( m_pImp, SfxInPlaceClient_Impl, TimerHdl ) );
    m_pImp->m_bStoreObject = sal_True;
}

//--------------------------------------------------------------------

SfxInPlaceClient::~SfxInPlaceClient()
{
    m_pViewSh->IPClientGone_Impl(this);

    // deleting the client before storing the object means discarding all changes
    m_pImp->m_bStoreObject = sal_False;
    SetObject(0);
    m_pImp->m_pClient = NULL;

    // the next call will destroy m_pImp if no other reference to it exists
    m_pImp->m_xClient = uno::Reference < embed::XEmbeddedClient >();

    // TODO/LATER:
    // the class is not intended to be used in multithreaded environment;
    // if it will this disconnection and all the parts that use the m_pClient
    // must be guarded with mutex
}

//--------------------------------------------------------------------
void SfxInPlaceClient::SetObjectState( sal_Int32 nState )
{
    if ( GetObject().is() )
    {
        try
        {
            GetObject()->changeState( nState );
        }
        catch ( uno::Exception& )
        {}
    }
}

//--------------------------------------------------------------------
sal_Int64 SfxInPlaceClient::GetObjectMiscStatus() const
{
    if ( GetObject().is() )
        return GetObject()->getStatus( m_pImp->m_nAspect );
    return 0;
}

//--------------------------------------------------------------------
uno::Reference < embed::XEmbeddedObject > SfxInPlaceClient::GetObject() const
{
    return m_pImp->m_xObject;
}

//--------------------------------------------------------------------
void SfxInPlaceClient::SetObject( const uno::Reference < embed::XEmbeddedObject >& rObject )
{
    if ( m_pImp->m_xObject.is() && rObject != m_pImp->m_xObject )
    {
        DBG_ASSERT( GetObject()->getClientSite() == m_pImp->m_xClient, "Wrong ClientSite!" );
        if ( GetObject()->getClientSite() == m_pImp->m_xClient )
        {
            if ( GetObject()->getCurrentState() != embed::EmbedStates::LOADED )
                SetObjectState( embed::EmbedStates::RUNNING );
            m_pImp->m_xObject->removeEventListener( uno::Reference < document::XEventListener >( m_pImp->m_xClient, uno::UNO_QUERY ) );
            m_pImp->m_xObject->removeStateChangeListener( uno::Reference < embed::XStateChangeListener >( m_pImp->m_xClient, uno::UNO_QUERY ) );
            m_pImp->m_xObject->setClientSite( 0 );
        }
    }

    m_pImp->m_xObject = rObject;

    if ( rObject.is() )
    {
        // as soon as an object was connected to a client it has to be checked wether the object wants
        // to be activated
        rObject->addStateChangeListener( uno::Reference < embed::XStateChangeListener >( m_pImp->m_xClient, uno::UNO_QUERY ) );
        rObject->addEventListener( uno::Reference < document::XEventListener >( m_pImp->m_xClient, uno::UNO_QUERY ) );
        rObject->setClientSite( m_pImp->m_xClient );
        m_pImp->m_aTimer.Start();
    }
    else
        m_pImp->m_aTimer.Stop();
}

//--------------------------------------------------------------------
BOOL SfxInPlaceClient::SetObjArea( const Rectangle& rArea )
{
    if( rArea != m_pImp->m_aObjArea )
    {
        m_pImp->m_aObjArea = rArea;
        m_pImp->SizeHasChanged();

        Invalidate();
        return TRUE;
    }

    return FALSE;
}

//--------------------------------------------------------------------
Rectangle SfxInPlaceClient::GetObjArea() const
{
    return m_pImp->m_aObjArea;
}

Rectangle SfxInPlaceClient::GetScaledObjArea() const
{
    Rectangle aRealObjArea( m_pImp->m_aObjArea );
    aRealObjArea.SetSize( Size( Fraction( aRealObjArea.GetWidth() ) * m_pImp->m_aScaleWidth,
                                Fraction( aRealObjArea.GetHeight() ) * m_pImp->m_aScaleHeight ) );
    return aRealObjArea;
}

//--------------------------------------------------------------------
void SfxInPlaceClient::SetSizeScale( const Fraction & rScaleWidth, const Fraction & rScaleHeight )
{
    if ( m_pImp->m_aScaleWidth != rScaleWidth || m_pImp->m_aScaleHeight != rScaleHeight )
    {
        m_pImp->m_aScaleWidth = rScaleWidth;
        m_pImp->m_aScaleHeight = rScaleHeight;

        m_pImp->SizeHasChanged();

        // TODO/LATER: Invalidate seems to trigger (wrong) recalculations of the ObjArea, so it's better
        // not to call it here, but maybe it sounds reasonable to do so.
        //Invalidate();
    }
}

//--------------------------------------------------------------------
const Fraction& SfxInPlaceClient::GetScaleWidth() const
{
    return m_pImp->m_aScaleWidth;
}

//--------------------------------------------------------------------
const Fraction& SfxInPlaceClient::GetScaleHeight() const
{
    return m_pImp->m_aScaleHeight;
}

//--------------------------------------------------------------------
void SfxInPlaceClient::Invalidate()
{
    // TODO/LATER: do we need both?
    awt::Rectangle aAwtRect = m_pImp->getPlacement();
    Rectangle aRect = VCLRectangle( aAwtRect );
    m_pEditWin->Invalidate( aRect );
    ViewChanged();
}

//--------------------------------------------------------------------
sal_Bool SfxInPlaceClient::IsObjectUIActive() const
{
    try {
        return ( m_pImp->m_xObject.is() && ( m_pImp->m_xObject->getCurrentState() == embed::EmbedStates::UI_ACTIVE ) );
    }
    catch( uno::Exception& )
    {}

    return sal_False;
}

//--------------------------------------------------------------------
sal_Bool SfxInPlaceClient::IsObjectInPlaceActive() const
{
    try {
        return ( m_pImp->m_xObject.is() && ( m_pImp->m_xObject->getCurrentState() == embed::EmbedStates::INPLACE_ACTIVE ) ||
                 m_pImp->m_xObject.is() && ( m_pImp->m_xObject->getCurrentState() == embed::EmbedStates::UI_ACTIVE ) );
    }
    catch( uno::Exception& )
    {}

    return sal_False;
}

//--------------------------------------------------------------------
sal_Bool SfxInPlaceClient::IsObjectActive() const
{
    try {
        return ( m_pImp->m_xObject.is() && ( m_pImp->m_xObject->getCurrentState() == embed::EmbedStates::ACTIVE ) );
    }
    catch( uno::Exception& )
    {}

    return sal_False;
}

//--------------------------------------------------------------------
Window* SfxInPlaceClient::GetActiveWindow( SfxObjectShell* pDoc, const com::sun::star::uno::Reference < com::sun::star::embed::XEmbeddedObject >& xObject )
{
    SfxInPlaceClient* pClient = GetClient( pDoc, xObject );
    if ( pClient )
        return pClient->GetEditWin();
    return NULL;
}

//--------------------------------------------------------------------
SfxInPlaceClient* SfxInPlaceClient::GetClient( SfxObjectShell* pDoc, const com::sun::star::uno::Reference < com::sun::star::embed::XEmbeddedObject >& xObject )
{
    for ( SfxViewFrame* pFrame = SfxViewFrame::GetFirst(pDoc); pFrame; pFrame=SfxViewFrame::GetNext(*pFrame,pDoc) )
    {
        SfxInPlaceClient* pClient = pFrame->GetViewShell()->FindIPClient( xObject, NULL );
        if ( pClient )
            return pClient;
    }

    return NULL;
}

sal_Int64 SfxInPlaceClient::GetAspect() const
{
    return embed::Aspects::MSOLE_CONTENT;
}

ErrCode SfxInPlaceClient::DoVerb( long nVerb )
{
    SfxErrorContext aEc( ERRCTX_SO_DOVERB, m_pViewSh->GetWindow(), RID_SO_ERRCTX );
    ErrCode nError = ERRCODE_NONE;
    if ( m_pImp->m_xObject.is() )
    {
        sal_Bool bSaveCopyAs = sal_False;
        if ( nVerb == -8 ) // "Save Copy as..."
        {
            svt::EmbeddedObjectRef::TryRunningState( m_pImp->m_xObject );
            // TODO/LATER: this special verb should disappear when outplace activation is completely available
            uno::Reference< frame::XModel > xEmbModel( m_pImp->m_xObject->getComponent(), uno::UNO_QUERY );
            if ( xEmbModel.is() )
            {
                bSaveCopyAs = sal_True;

                try
                {
                    uno::Reference< lang::XMultiServiceFactory > xEmptyFactory;
                    SfxStoringHelper aHelper( xEmptyFactory );
                    uno::Sequence< beans::PropertyValue > aDispatchArgs( 1 );
                    aDispatchArgs[0].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "SaveTo" ) );
                    aDispatchArgs[0].Value <<= (sal_Bool)sal_True;

                    aHelper.GUIStoreModel( xEmbModel,
                                            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "SaveAs" ) ),
                                            aDispatchArgs );
                }
                catch( uno::Exception& )
                {
                    nError = ERRCODE_IO_GENERAL;
                    // TODO/LATER: better error handling
                }
            }
        }

        if ( !bSaveCopyAs )
        {
            try
            {
                m_pImp->m_xObject->doVerb( nVerb );
            }
            catch ( embed::UnreachableStateException& )
            {
                if ( nVerb == 0 )
                {
                    // a workaround for the default verb, usually makes sence for alien objects
                    try
                    {
                        m_pImp->m_xObject->doVerb( -9 ); // open own view, a workaround verb that is not visible
                    }
                    catch ( uno::Exception& )
                    {
                        nError = ERRCODE_SO_GENERALERROR;
                    }
                }
            }
            catch ( uno::Exception& )
            {
                nError = ERRCODE_SO_GENERALERROR;
                //TODO/LATER: better error handling
            }
        }
    }

    if( nError )
        ErrorHandler::HandleError( nError );
    return nError;
}

void SfxInPlaceClient::VisAreaChanged()
{
    uno::Reference < embed::XInplaceObject > xObj( m_pImp->m_xObject, uno::UNO_QUERY );
    uno::Reference < embed::XInplaceClient > xClient( m_pImp->m_xClient, uno::UNO_QUERY );
    if ( xObj.is() && xClient.is() )
        xObj->setObjectRectangles( xClient->getPlacement(), xClient->getClipRectangle() );
}

