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


Listener::Listener( sd::Transmitter *aTransmitter  )
    : ::cppu::WeakComponentImplHelper1< XSlideShowListener>( m_aMutex )
{
    fprintf( stderr, "Creating Transmitter\n" );
    mTransmitter = aTransmitter;
}

Listener::~Listener()
{
}

void Listener::init(css::uno::Reference< css::presentation::XSlideShowController >& aController)
{
    fprintf( stderr, "Initing Transmitter\n" );
    if (aController.is() )
    {
//     mController = aController;
    aController->addSlideShowListener(static_cast<XSlideShowListener*>(this));-
    }
    fprintf( stderr, "Initiated Transmitter\n" );
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
    fprintf( stderr, "slideEnded\n" );
    (void) bReverse;
    sal_Int32 aSlide = mController->getCurrentSlideIndex();

    OStringBuffer aBuilder( "slide_updated\n" );
    aBuilder.append( OString::valueOf( aSlide ) );
    aBuilder.append( "\n\n" );

    mTransmitter->addMessage( aBuilder.makeStringAndClear() ,
                              Transmitter::Priority::HIGH );
}

void SAL_CALL Listener::hyperLinkClicked (const rtl::OUString &)
    throw (css::uno::RuntimeException)
{
}

void SAL_CALL Listener::slideTransitionStarted (void)
    throw (css::uno::RuntimeException)
{
        fprintf( stderr, "slideTransitionStarted\n" );
}

void SAL_CALL Listener::slideTransitionEnded (void)
    throw (css::uno::RuntimeException)
{
    fprintf( stderr, "slideTransitionEnded\n" );
}

void SAL_CALL Listener::slideAnimationsEnded (void)
    throw (css::uno::RuntimeException)
{
    fprintf( stderr, "slideAnimationsEnded\n" );
}

void SAL_CALL Listener::disposing (void)
{
// FIXME: disconnect as appropriate
}

void SAL_CALL Listener::disposing (
    const css::lang::EventObject& rEvent)
    throw (::com::sun::star::uno::RuntimeException)
{
    (void) rEvent;
// FIXME: disconnect as appropriate
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */