/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: cancelcommandexecution.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 16:36:02 $
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

/**************************************************************************
                                TODO
 **************************************************************************

 *************************************************************************/

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _CPPUHELPER_EXC_HLP_HXX_
#include <cppuhelper/exc_hlp.hxx>
#endif
#ifndef _COM_SUN_STAR_UCB_COMMANDFAILEDEXCEPTION_HPP_
#include <com/sun/star/ucb/CommandFailedException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCOMMANDENVIRONMENT_HPP_
#include <com/sun/star/ucb/XCommandEnvironment.hpp>
#endif
#ifndef _UCBHELPER_INTERACTIONREQUEST_HXX
#include <ucbhelper/interactionrequest.hxx>
#endif
#ifndef _UCBHELPER_CANCELCOMMANDEXECUTION_HXX_
#include <ucbhelper/cancelcommandexecution.hxx>
#endif
#ifndef _UCBHELPER_SIMPLEIOERRORREQUEST_HXX
#include <ucbhelper/simpleioerrorrequest.hxx>
#endif

using namespace com::sun::star;

namespace ucbhelper
{

//=========================================================================
void cancelCommandExecution( const uno::Any & rException,
                             const uno::Reference<
                                        ucb::XCommandEnvironment > & xEnv )
    throw( uno::Exception )
{
    if ( xEnv.is() )
    {
        uno::Reference<
            task::XInteractionHandler > xIH = xEnv->getInteractionHandler();
        if ( xIH.is() )
        {
            rtl::Reference< ucbhelper::InteractionRequest > xRequest
                = new ucbhelper::InteractionRequest( rException );

            uno::Sequence< uno::Reference< task::XInteractionContinuation > >
                aContinuations( 1 );
            aContinuations[ 0 ]
                = new ucbhelper::InteractionAbort( xRequest.get() );

            xRequest->setContinuations( aContinuations );

            xIH->handle( xRequest.get() );

            rtl::Reference< ucbhelper::InteractionContinuation > xSelection
                = xRequest->getSelection();

            if ( xSelection.is() )
                throw ucb::CommandFailedException(
                                    rtl::OUString(),
                                    uno::Reference< uno::XInterface >(),
                                    rException );
        }
    }

    cppu::throwException( rException );

    OSL_ENSURE( sal_False, "Return from cppu::throwException call!!!" );
    throw uno::RuntimeException();
}

#if SUPD < 641
//=========================================================================
void cancelCommandExecution( const ucb::IOErrorCode eError,
                             const rtl::OUString & rArg,
                             const uno::Reference<
                                ucb::XCommandEnvironment > & xEnv,
                             const rtl::OUString & rMessage,
                             const uno::Reference<
                                    ucb::XCommandProcessor > & xContext )
    throw( uno::Exception )
{
    uno::Sequence< uno::Any > aArgs( 1 );
    aArgs[ 0 ] <<= rArg;

    rtl::Reference< ucbhelper::SimpleIOErrorRequest > xRequest
        = new ucbhelper::SimpleIOErrorRequest(
                                    eError, aArgs, rMessage, xContext );
    if ( xEnv.is() )
    {
        uno::Reference<
            task::XInteractionHandler > xIH = xEnv->getInteractionHandler();
        if ( xIH.is() )
        {
            xIH->handle( xRequest.get() );

            rtl::Reference< ucbhelper::InteractionContinuation > xSelection
                = xRequest->getSelection();

            if ( xSelection.is() )
                throw ucb::CommandFailedException( rtl::OUString(),
                                                   xContext,
                                                   xRequest->getRequest() );
        }
    }

    cppu::throwException( xRequest->getRequest() );

    OSL_ENSURE( sal_False, "Return from cppu::throwException call!!!" );
    throw uno::RuntimeException();
}

//=========================================================================
void cancelCommandExecution( const ucb::IOErrorCode eError,
                             const rtl::OUString & rArg1,
                             const rtl::OUString & rArg2,
                             const uno::Reference<
                                ucb::XCommandEnvironment > & xEnv,
                             const rtl::OUString & rMessage,
                             const uno::Reference<
                                    ucb::XCommandProcessor > & xContext )
    throw( uno::Exception )
{
    uno::Sequence< uno::Any > aArgs( 2 );
    aArgs[ 0 ] <<= rArg1;
    aArgs[ 1 ] <<= rArg2;

    rtl::Reference< ucbhelper::SimpleIOErrorRequest > xRequest
        = new ucbhelper::SimpleIOErrorRequest(
                                    eError, aArgs, rMessage, xContext );
    if ( xEnv.is() )
    {
        uno::Reference<
            task::XInteractionHandler > xIH = xEnv->getInteractionHandler();
        if ( xIH.is() )
        {
            xIH->handle( xRequest.get() );

            rtl::Reference< ucbhelper::InteractionContinuation > xSelection
                = xRequest->getSelection();

            if ( xSelection.is() )
                throw ucb::CommandFailedException( rtl::OUString(),
                                                   xContext,
                                                   xRequest->getRequest() );
        }
    }

    cppu::throwException( xRequest->getRequest() );

    OSL_ENSURE( sal_False, "Return from cppu::throwException call!!!" );
    throw uno::RuntimeException();
}
#endif // SUPD, 641

//=========================================================================
void cancelCommandExecution( const ucb::IOErrorCode eError,
                             const uno::Sequence< uno::Any > & rArgs,
                             const uno::Reference<
                                ucb::XCommandEnvironment > & xEnv,
                             const rtl::OUString & rMessage,
                             const uno::Reference<
                                    ucb::XCommandProcessor > & xContext )
    throw( uno::Exception )
{
    rtl::Reference< ucbhelper::SimpleIOErrorRequest > xRequest
        = new ucbhelper::SimpleIOErrorRequest(
                                    eError, rArgs, rMessage, xContext );
    if ( xEnv.is() )
    {
        uno::Reference<
            task::XInteractionHandler > xIH = xEnv->getInteractionHandler();
        if ( xIH.is() )
        {
            xIH->handle( xRequest.get() );

            rtl::Reference< ucbhelper::InteractionContinuation > xSelection
                = xRequest->getSelection();

            if ( xSelection.is() )
                throw ucb::CommandFailedException( rtl::OUString(),
                                                   xContext,
                                                   xRequest->getRequest() );
        }
    }

    cppu::throwException( xRequest->getRequest() );

    OSL_ENSURE( sal_False, "Return from cppu::throwException call!!!" );
    throw uno::RuntimeException();
}

}
