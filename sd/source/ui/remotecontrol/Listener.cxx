/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <comphelper/processfactory.hxx>
#include <com/sun/star/presentation/XPresentationSupplier.hpp>
#include <com/sun/star/presentation/XPresentation2.hpp>

#include <rtl/strbuf.hxx>

#include "Listener.hxx"

using namespace sd;
using namespace ::com::sun::star::presentation;
using namespace ::com::sun::star::frame;
using rtl::OString;
using rtl::OStringBuffer;


Listener::Listener( const ::rtl::Reference<Server>& rServer, sd::Transmitter *aTransmitter  )
    : ::cppu::WeakComponentImplHelper1< XSlideShowListener >( m_aMutex ),
      mServer( rServer ),
      pTransmitter( NULL ),
      mPreparer()
{
    pTransmitter = aTransmitter;
    fprintf( stderr, "Listener created %p\n", this );
}

Listener::~Listener()
{
    fprintf( stderr, "Listener destroyed %p\n", this );
}

void Listener::init( const css::uno::Reference< css::presentation::XSlideShowController >& aController)
{
    fprintf( stderr, "Initing\n" );
    if ( aController.is() )
    {
        fprintf( stderr, "Is -- now copying refrerence.\n" );
        mController = css::uno::Reference< css::presentation::XSlideShowController >( aController );
        fprintf( stderr, "Registering listener\n" );
        aController->addSlideShowListener( this );
        fprintf( stderr, "Registered listener.\n" );

        sal_Int32 aSlides = aController->getSlideCount();
        OStringBuffer aBuffer;
        aBuffer.append( "slideshow_started\n" )
               .append( OString::valueOf( aSlides ) ).append( "\n\n" );

        pTransmitter->addMessage( aBuffer.makeStringAndClear(),
                                  Transmitter::Priority::HIGH );

        mPreparer.set( new ImagePreparer( aController, pTransmitter, mPreparer ) );
        mPreparer->launch();
    }
    else
    {
        fprintf( stderr, "Couldn't register listener -- aController isn't\n" );
    }
}

//----- XAnimationListener ----------------------------------------------------
void SAL_CALL Listener::beginEvent(const css::uno::Reference<
    css::animations::XAnimationNode >&  rNode ) throw (css::uno::RuntimeException)
{
    (void) rNode;
}


void SAL_CALL Listener::endEvent( const css::uno::Reference<
    css::animations::XAnimationNode >& rNode ) throw (css::uno::RuntimeException)
{
    (void) rNode;
}

void SAL_CALL Listener::repeat( const css::uno::Reference<
    css::animations::XAnimationNode >& rNode, ::sal_Int32 aRepeat )
     throw (css::uno::RuntimeException)
{
    (void) rNode;
    (void) aRepeat;
}


//----- XSlideShowListener ----------------------------------------------------

void SAL_CALL Listener::paused (void)
    throw (com::sun::star::uno::RuntimeException)
{
}

void SAL_CALL Listener::resumed (void)
    throw (css::uno::RuntimeException)
{
}

void SAL_CALL Listener::slideEnded (sal_Bool bReverse)
    throw (css::uno::RuntimeException)
{
    (void) bReverse;
}

void SAL_CALL Listener::hyperLinkClicked (const rtl::OUString &)
    throw (css::uno::RuntimeException)
{
}

void SAL_CALL Listener::slideTransitionStarted (void)
    throw (css::uno::RuntimeException)
{
    fprintf( stderr, "slideTransitionStarted\n" );
    sal_Int32 aSlide = mController->getCurrentSlideIndex();

    OStringBuffer aBuilder( "slide_updated\n" );
    aBuilder.append( OString::valueOf( aSlide ) );
    aBuilder.append( "\n\n" );

    if ( pTransmitter )
    {
        pTransmitter->addMessage( aBuilder.makeStringAndClear(),
                               Transmitter::Priority::HIGH );
    }
}

void SAL_CALL Listener::slideTransitionEnded (void)
    throw (css::uno::RuntimeException)
{
}

void SAL_CALL Listener::slideAnimationsEnded (void)
    throw (css::uno::RuntimeException)
{
}

void SAL_CALL Listener::disposing (void)
{
    fprintf( stderr, "disposing void\n" );
    if ( mPreparer.is() )
    {
        delete mPreparer.get();
        mPreparer = NULL;
    }

    pTransmitter = NULL;
    if ( mController.is() )
    {
        mController->removeSlideShowListener( this );
        mController = NULL;
    }
    mServer->informListenerDestroyed();
}

void SAL_CALL Listener::disposing (
    const css::lang::EventObject& rEvent)
    throw (::com::sun::star::uno::RuntimeException)
{
    fprintf( stderr, "disposing with Events\n" );
    (void) rEvent;
    dispose();
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */