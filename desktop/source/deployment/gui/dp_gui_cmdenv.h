/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dp_gui_cmdenv.h,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: jl $ $Date: 2006-12-22 09:00:28 $
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

#if ! defined INCLUDED_DP_GUI_CMDENV_H
#define INCLUDED_DP_GUI_CMDENV_H

#include "tools/resmgr.hxx"
#include "osl/conditn.hxx"
#include "cppuhelper/implbase3.hxx"
#include "vcl/dialog.hxx"
#include "vcl/button.hxx"
#include "vcl/fixed.hxx"
#include "vcl/status.hxx"
#include "com/sun/star/deployment/thePackageManagerFactory.hpp"
#include <memory>

namespace dp_gui
{

struct DialogImpl;

//==============================================================================
//Only if the class is consructed with the DialogImpl then the ProgressDialog can be
//displayed. Otherwise this class can still be used to forward an interaction. This
//is done by the interaction handler of the "Download and Installation" dialog.

class ProgressCommandEnv
    : public ::cppu::WeakImplHelper3< ::com::sun::star::ucb::XCommandEnvironment,
                                      ::com::sun::star::task::XInteractionHandler,
                                      ::com::sun::star::ucb::XProgressHandler >
{
    ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler> m_xHandler;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > m_xContext;
    Dialog * m_dialog;
    ::rtl::OUString m_title;
       bool m_bAskWhenInstalling;
    sal_Int32 m_currentInnerProgress;
    sal_Int32 m_currentProgressSection;
    sal_Int32 m_progressSections;
    void updateProgress( ::rtl::OUString const & text = ::rtl::OUString() );
    ::com::sun::star::uno::Reference< ::com::sun::star::task::XAbortChannel> m_xAbortChannel;
    bool m_aborted;

    struct ProgressDialog : public Dialog
    {
        struct CancelButtonImpl : public CancelButton
        {
            ProgressDialog * m_dialog;
            inline CancelButtonImpl( ProgressDialog * dialog )
                : CancelButton( dialog ),
                  m_dialog( dialog )
                {}
            virtual void Click();
        };
        friend struct CancelButtonImpl;

        ProgressCommandEnv * m_cmdEnv;
        ::std::auto_ptr<FixedText> m_ftCurrentAction;
        ::std::auto_ptr<StatusBar> m_statusBar;
        ::std::auto_ptr<CancelButtonImpl> m_cancelButton;

        ProgressDialog( ProgressCommandEnv * cmdEnv );
    };
    friend struct ProgressDialog;
    friend struct ProgressDialog::CancelButtonImpl;

    ::std::auto_ptr<ProgressDialog> m_progressDialog;
    DECL_LINK( executeDialog, ::osl::Condition * );

    void update_( ::com::sun::star::uno::Any const & Status )
        throw (::com::sun::star::uno::RuntimeException);

    void solarthread_dtor();

    Dialog * activeDialog(); // either m_progressDialog or m_dialog

public:
    virtual ~ProgressCommandEnv();

    // When param bAskWhenInstalling = true, then the user is asked if he
    //agrees to install this extension.
    inline ProgressCommandEnv(
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > ctx,
        Dialog * dialog,
        ::rtl::OUString const & title,
        bool bAskWhenInstalling = false):
        m_xContext(ctx),
        m_dialog( dialog ),
        m_title( title ),
        m_bAskWhenInstalling(bAskWhenInstalling),
        m_aborted( false )
        {}

    void showProgress( sal_Int32 progressSections );
    void progressSection(
        String const & text,
        ::com::sun::star::uno::Reference< ::com::sun::star::task::XAbortChannel > const & xAbortChannel = 0 );
    inline bool isAborted() const { return m_aborted; }

    // XCommandEnvironment
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler > SAL_CALL
    getInteractionHandler() throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XProgressHandler >
    SAL_CALL getProgressHandler() throw (::com::sun::star::uno::RuntimeException);

    // XInteractionHandler
    virtual void SAL_CALL handle(
        ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionRequest > const & xRequest )
        throw (::com::sun::star::uno::RuntimeException);

    // XProgressHandler
    virtual void SAL_CALL push( ::com::sun::star::uno::Any const & Status )
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL update( ::com::sun::star::uno::Any const & Status )
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL pop() throw (::com::sun::star::uno::RuntimeException);
};

}

#endif
