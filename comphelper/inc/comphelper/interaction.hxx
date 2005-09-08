/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: interaction.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 02:32:06 $
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

#ifndef _COMPHELPER_INTERACTION_HXX_
#define _COMPHELPER_INTERACTION_HXX_

#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif
#ifndef _COM_SUN_STAR_TASK_XINTERACTIONAPPROVE_HPP_
#include <com/sun/star/task/XInteractionApprove.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XINTERACTIONDISAPPROVE_HPP_
#include <com/sun/star/task/XInteractionDisapprove.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XINTERACTIONABORT_HPP_
#include <com/sun/star/task/XInteractionAbort.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XINTERACTIONRETRY_HPP_
#include <com/sun/star/task/XInteractionRetry.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XINTERACTIONREQUEST_HPP_
#include <com/sun/star/task/XInteractionRequest.hpp>
#endif
#ifndef INCLUDED_COMPHELPERDLLAPI_H
#include "comphelper/comphelperdllapi.h"
#endif

//.........................................................................
namespace comphelper
{
//.........................................................................

    //=========================================================================
    //= OInteractionSelect
    //=========================================================================
    /** base class for concrete XInteractionContinuation implementations.<p/>
        Instances of the classes maintain a flag indicating if the handler was called.
    */
    class OInteractionSelect
    {
        sal_Bool    m_bSelected : 1;    /// indicates if the select event occured

    protected:
        OInteractionSelect() : m_bSelected(sal_False) { }

    public:
        /// determines whether or not this handler was selected
        sal_Bool    wasSelected() const { return m_bSelected; }
        /// resets the state to "not selected", so you may reuse the handler
        void        reset() { m_bSelected = sal_False; }

    protected:
        void    implSelected() { m_bSelected = sal_True; }
    };

    //=========================================================================
    //= OInteraction
    //=========================================================================
    /** template for instantiating concret interaction handlers<p/>
        the template argument must eb an interface derived from XInteractionContinuation
    */
    template <class INTERACTION>
    class OInteraction
            :public ::cppu::WeakImplHelper1< INTERACTION >
            ,public OInteractionSelect
    {
    public:
        OInteraction() { }

    // XInteractionContinuation
        virtual void SAL_CALL select(  ) throw(::com::sun::star::uno::RuntimeException);
    };

    //.........................................................................
    template <class INTERACTION>
    void SAL_CALL OInteraction< INTERACTION >::select(  ) throw(::com::sun::star::uno::RuntimeException)
    {
        implSelected();
    }

    //=========================================================================
    //= OInteractionApprove
    //=========================================================================
    typedef OInteraction< ::com::sun::star::task::XInteractionApprove > OInteractionApprove;

    //=========================================================================
    //= OInteractionDispprove
    //=========================================================================
    typedef OInteraction< ::com::sun::star::task::XInteractionDisapprove >  OInteractionDisapprove;

    //=========================================================================
    //= OInteractionAbort
    //=========================================================================
    typedef OInteraction< ::com::sun::star::task::XInteractionAbort >   OInteractionAbort;

    //=========================================================================
    //= OInteractionRetry
    //=========================================================================
    typedef OInteraction< ::com::sun::star::task::XInteractionRetry >   OInteractionRetry;

    //=========================================================================
    //= OInteractionRequest
    //=========================================================================
    typedef ::cppu::WeakImplHelper1 <   ::com::sun::star::task::XInteractionRequest
                                    >   OInteractionRequest_Base;
    /** implements an interaction request (<type scope="com.sun.star.task">XInteractionRequest</type>)<p/>
        at run time, you can freely add any interaction continuation objects
    */
    class COMPHELPER_DLLPUBLIC OInteractionRequest : public OInteractionRequest_Base
    {
        ::com::sun::star::uno::Any
                    m_aRequest;         /// the request we represent
        ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation > >
                    m_aContinuations;   /// all registered continuations

    public:
        OInteractionRequest(const ::com::sun::star::uno::Any& _rRequestDescription);

        /// add a new continuation
        void addContinuation(const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation >& _rxContinuation);
        /// clear all continuations
        void clearContinuations();

    // XInteractionRequest
        virtual ::com::sun::star::uno::Any SAL_CALL getRequest(  ) throw(::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation > > SAL_CALL getContinuations(  ) throw(::com::sun::star::uno::RuntimeException);
    };
//.........................................................................
}   // namespace comphelper
//.........................................................................

#endif // _COMPHELPER_INTERACTION_HXX_


