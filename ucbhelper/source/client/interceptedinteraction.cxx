/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: interceptedinteraction.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 12:11:52 $
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

#ifndef _UCBHELPER_INTERCEPTEDINTERACTION_HXX_
#include <ucbhelper/interceptedinteraction.hxx>
#endif

//_______________________________________________
// includes

//_______________________________________________
// namespace

namespace ucbhelper{

namespace css = ::com::sun::star;

//_______________________________________________
// definitions

/*-----------------------------------------------
    17.03.2004 11:00
-----------------------------------------------*/
InterceptedInteraction::InterceptedInteraction()
{
}

/*-----------------------------------------------
    17.03.2004 14:55
-----------------------------------------------*/
void InterceptedInteraction::setInterceptedHandler(const css::uno::Reference< css::task::XInteractionHandler >& xInterceptedHandler)
{
    m_xInterceptedHandler = xInterceptedHandler;
}

/*-----------------------------------------------
    17.03.2004 14:55
-----------------------------------------------*/
void InterceptedInteraction::setInterceptions(const ::std::vector< InterceptedRequest >& lInterceptions)
{
    m_lInterceptions = lInterceptions;
}

/*-----------------------------------------------
    18.03.2004 10:10
-----------------------------------------------*/
InterceptedInteraction::EInterceptionState InterceptedInteraction::intercepted(
    const InterceptedRequest&,
    const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionRequest >&)
{
    // default behaviour! see impl_interceptRequest() for further informations ...
    return E_NOT_INTERCEPTED;
}

/*-----------------------------------------------
    18.03.2004 09:46
-----------------------------------------------*/
css::uno::Reference< css::task::XInteractionContinuation > InterceptedInteraction::extractContinuation(const css::uno::Sequence< css::uno::Reference< css::task::XInteractionContinuation > >& lContinuations,
                                                                                                       const css::uno::Type&                                                                   aType         )
{
    const css::uno::Reference< css::task::XInteractionContinuation >* pContinuations = lContinuations.getConstArray();

    sal_Int32 c = lContinuations.getLength();
    sal_Int32 i = 0;

    for (i=0; i<c; ++i)
    {
        css::uno::Reference< css::uno::XInterface > xCheck(pContinuations[i], css::uno::UNO_QUERY);
        if (xCheck->queryInterface(aType).hasValue())
            return pContinuations[i];
    }

    return css::uno::Reference< css::task::XInteractionContinuation >();
}

/*-----------------------------------------------
    18.03.2004 10:03
-----------------------------------------------*/
void SAL_CALL InterceptedInteraction::handle(const css::uno::Reference< css::task::XInteractionRequest >& xRequest)
    throw(css::uno::RuntimeException)
{
    impl_handleDefault(xRequest);
}

/*-----------------------------------------------
    18.03.2004 10:02
-----------------------------------------------*/
void InterceptedInteraction::impl_handleDefault(const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionRequest >& xRequest)
{
    EInterceptionState eState = impl_interceptRequest(xRequest);

    switch(eState)
    {
        case E_NOT_INTERCEPTED:
        {
            // Non of the intercepted requests match to the given one.
            // => forward request to the internal wrapped handler - if there is one.
            if (m_xInterceptedHandler.is())
                m_xInterceptedHandler->handle(xRequest);
        }
        break;

        case E_NO_CONTINUATION_FOUND:
        {
            // Runtime error! The defined continuation could not be located
            // inside the set of available containuations of the incoming request.
            // Whats wrong - the interception list or the request?
            OSL_ENSURE(sal_False, "InterceptedInteraction::handle()\nCould intercept this interaction request - but cant locate the right continuation!");
        }
        break;

        case E_INTERCEPTED:
        break;
    }
}

/*-----------------------------------------------
    18.03.2004 09:48
-----------------------------------------------*/
InterceptedInteraction::EInterceptionState InterceptedInteraction::impl_interceptRequest(const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionRequest >& xRequest)
{
    css::uno::Any                                                                    aRequest       = xRequest->getRequest();
    css::uno::Type                                                                   aRequestType   = aRequest.getValueType();
    css::uno::Sequence< css::uno::Reference< css::task::XInteractionContinuation > > lContinuations = xRequest->getContinuations();

    // check against the list of static requests
    sal_Int32 nHandle = 0;
    ::std::vector< InterceptedRequest >::const_iterator pIt;
    for (  pIt  = m_lInterceptions.begin();
           pIt != m_lInterceptions.end()  ;
         ++pIt                            )
    {
        const InterceptedRequest& rInterception = *pIt;
        css::uno::Type aInterceptedType = rInterception.Request.getValueType();

        // check the request
        sal_Bool bMatch = sal_False;
        if (rInterception.MatchExact)
            bMatch = aInterceptedType.equals(aRequestType);
        else
            bMatch = aInterceptedType.isAssignableFrom(aRequestType); // dont change intercepted and request type here -> it will check the wrong direction!

        // intercepted ...
        // Call they might existing derived class, so they can handle that by its own.
        // If its not interested on that (may be its not overwritten and the default implementation
        // returns E_NOT_INTERCEPTED as default) -> break this loop and search for the right continuation.
        if (bMatch)
        {
            EInterceptionState eState = intercepted(rInterception, xRequest);
            if (eState == E_NOT_INTERCEPTED)
                break;
            return eState;
        }

        ++nHandle;
    }

    if (pIt != m_lInterceptions.end()) // => can be true only if bMatch=TRUE!
    {
        // match -> search required continuation
        const InterceptedRequest& rInterception = *pIt;
        css::uno::Reference< css::task::XInteractionContinuation > xContinuation = InterceptedInteraction::extractContinuation(lContinuations, rInterception.Continuation);
        if (xContinuation.is())
        {
            xContinuation->select();
            return E_INTERCEPTED;
        }

        // Can be reached only, if the request does not support the given continuation!
        // => RuntimeError!?
        return E_NO_CONTINUATION_FOUND;
    }

    return E_NOT_INTERCEPTED;
}

} // namespace ucbhelper
