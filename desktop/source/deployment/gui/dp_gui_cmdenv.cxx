/*************************************************************************
 *
 *  $RCSfile: dp_gui_cmdenv.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: kz $ $Date: 2005-01-21 17:10:23 $
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
 *  Copyright: 2002 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#include "dp_gui.hrc"
#include "dp_gui.h"
#include "dp_gui_cmdenv.h"
#include "comphelper/anytostring.hxx"
#include "com/sun/star/lang/WrappedTargetException.hpp"
#include "com/sun/star/beans/PropertyValue.hpp"
#include "com/sun/star/task/XInteractionAbort.hpp"
#include "com/sun/star/task/XInteractionApprove.hpp"
#include "vcl/msgbox.hxx"

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::ucb;
using ::rtl::OUString;

namespace dp_gui
{

//______________________________________________________________________________
void ProgressCommandEnv::ProgressDialog::CancelButtonImpl::Click()
{
    m_dialog->m_cmdEnv->m_aborted = true;
    if (m_dialog->m_cmdEnv->m_xAbortChannel.is())
    {
        try {
            m_dialog->m_cmdEnv->m_xAbortChannel->sendAbort();
        }
        catch (RuntimeException &) {
            OSL_ENSURE( 0, "### unexpected RuntimeException!" );
        }
    }
}

//______________________________________________________________________________
ProgressCommandEnv::~ProgressCommandEnv()
{
    if (m_progressDialog.get() != 0) {
        const ::vos::OGuard guard( Application::GetSolarMutex() );
        m_progressDialog->SetModalInputMode( FALSE );
        m_progressDialog.reset();
    }
}

//______________________________________________________________________________
ProgressCommandEnv::ProgressDialog::ProgressDialog(
    ProgressCommandEnv * cmdEnv )
    : Dialog( cmdEnv->m_mainDialog, WB_STDMODAL ),
      m_cmdEnv( cmdEnv )
{
    SetModalInputMode( TRUE );
}

//______________________________________________________________________________
IMPL_LINK( ProgressCommandEnv, executeDialog, ::osl::Condition *, pCond )
{
    {
        ::std::auto_ptr<ProgressDialog> that( new ProgressDialog( this ) );
        that->SetHelpId( HID_PACKAGE_MANAGER_PROGRESS );

        that->SetStyle( that->GetStyle() & ~WB_CLOSEABLE ); // non-closeable
        that->SetText( m_title );

        long totalWidth = that->LogicToPixel(
            Size( 250, 0 ), MapMode( MAP_APPFONT ) ).getWidth();
        long barWidth = totalWidth -
            (2 * m_mainDialog->m_borderLeftTopSpace.getWidth());
        long posY = m_mainDialog->m_borderLeftTopSpace.getHeight();

        that->m_ftCurrentAction.reset( new FixedText( that.get() ) );
        that->m_ftCurrentAction->SetPosSizePixel(
            m_mainDialog->m_borderLeftTopSpace.getWidth(), posY,
            barWidth, m_mainDialog->m_ftFontHeight );
        posY += (m_mainDialog->m_ftFontHeight +
                 m_mainDialog->m_descriptionYSpace);

        that->m_statusBar.reset(
            new StatusBar( that.get(), WB_LEFT | WB_3DLOOK ) );
        that->m_statusBar->SetPosSizePixel(
            m_mainDialog->m_borderLeftTopSpace.getWidth(), posY,
            barWidth, m_mainDialog->m_ftFontHeight + 4 );
        posY += (m_mainDialog->m_ftFontHeight + 4 +
                 m_mainDialog->m_unrelatedSpace.getHeight());

        that->m_cancelButton.reset(
            new ProgressDialog::CancelButtonImpl( that.get() ) );
        that->m_cancelButton->SetHelpId( HID_PACKAGE_MANAGER_PROGRESS_CANCEL );
        that->m_cancelButton->SetPosSizePixel(
            (totalWidth - m_mainDialog->m_buttonSize.getWidth()) / 2,
            posY,
            m_mainDialog->m_buttonSize.getWidth(),
            m_mainDialog->m_buttonSize.getHeight() );
        posY += m_mainDialog->m_buttonSize.getHeight();

        that->SetSizePixel(
            Size( totalWidth,
                  posY + m_mainDialog->m_borderRightBottomSpace.getHeight() ) );

        that->m_ftCurrentAction->Show();
        that->m_statusBar->Show();
        that->m_cancelButton->Show();
        that->Show();
        m_progressDialog = that;
    }

    pCond->set();
    return 0;
}

//______________________________________________________________________________
void ProgressCommandEnv::showProgress( sal_Int32 progressSections )
{
    m_progressSections = progressSections;
    m_currentProgressSection = 0;
    m_currentInnerProgress = 0;
    if (m_progressDialog.get() == 0) {
        ::osl::Condition cond;
        Application::PostUserEvent(
            LINK( this, ProgressCommandEnv, executeDialog ), &cond );
        cond.wait();
    }
}

//______________________________________________________________________________
void ProgressCommandEnv::progressSection(
    String const & text, Reference<task::XAbortChannel> const & xAbortChannel )
{
    m_xAbortChannel = xAbortChannel;
    if (! m_aborted)
    {
        ++m_currentProgressSection;
        m_currentInnerProgress = 0;
        if (m_progressDialog.get() != 0) {
            const ::vos::OGuard guard( Application::GetSolarMutex() );
            if (m_progressDialog->m_statusBar->IsProgressMode())
                m_progressDialog->m_statusBar->EndProgressMode();
            m_progressDialog->m_statusBar->StartProgressMode( text );
            m_progressDialog->m_ftCurrentAction->SetText( String() );
            updateProgress();
        }
    }
}

//______________________________________________________________________________
void ProgressCommandEnv::updateProgress( OUString const & text )
{
    if (m_progressDialog.get() != 0)
    {
        const ::vos::OGuard guard( Application::GetSolarMutex() );
        if (text.getLength() > 0)
            m_progressDialog->m_ftCurrentAction->SetText( text );
        // xxx todo: how to do better?
        m_progressDialog->m_statusBar->SetProgressValue(
            static_cast<USHORT>(
                (((m_currentProgressSection - 1) +
                  (m_currentInnerProgress >= 20
                   ? 0.99 : (double)m_currentInnerProgress / 20.0)) * 100)
                / m_progressSections ) % 101 );
//             static_cast<USHORT>(
//                 ((m_currentProgressSection +
//                   (1.0 - (m_currentInnerProgress == 0
//                           ? 1.0 : 1.0 / m_currentInnerProgress))) * 100)
//                 / m_progressSections ) % 101 );
    }
}

// XCommandEnvironment
//______________________________________________________________________________
Reference<task::XInteractionHandler> ProgressCommandEnv::getInteractionHandler()
    throw (RuntimeException)
{
    return this;
}

//______________________________________________________________________________
Reference<XProgressHandler> ProgressCommandEnv::getProgressHandler()
    throw (RuntimeException)
{
    return this;
}

// XInteractionHandler
//______________________________________________________________________________
void ProgressCommandEnv::handle(
    Reference<task::XInteractionRequest> const & xRequest )
    throw (RuntimeException)
{
    Any request( xRequest->getRequest() );
    OSL_ASSERT( request.getValueTypeClass() == TypeClass_EXCEPTION );
#if OSL_DEBUG_LEVEL > 1
    OSL_TRACE( "[dp_gui_cmdenv.cxx] incoming request:\n%s\n",
               ::rtl::OUStringToOString( ::comphelper::anyToString(request),
                                         RTL_TEXTENCODING_UTF8 ).getStr() );
#endif

    lang::WrappedTargetException wtExc;
    if (request >>= wtExc) {
        // handable deployment error signalled, e.g.
        // bundle item registration failed, notify cause only:
        Any cause;
        deployment::DeploymentException dpExc;
        if (wtExc.TargetException >>= dpExc)
            cause = dpExc.Cause;
        else {
            CommandFailedException cfExc;
            if (wtExc.TargetException >>= cfExc)
                cause = cfExc.Reason;
            else
                cause = wtExc.TargetException;
        }
        update_( cause );

        // selections:
        bool approve = false;
        bool abort = false;

        // ignore intermediate errors of legacy packages, i.e.
        // former pkgchk behaviour:
        const Reference<deployment::XPackage> xPackage(
            wtExc.Context, UNO_QUERY );
        OSL_ASSERT( xPackage.is() );
        if (xPackage.is()) {
            const Reference<deployment::XPackageTypeInfo> xPackageType(
                xPackage->getPackageType() );
            OSL_ASSERT( xPackageType.is() );
            if (xPackageType.is()) {
                approve = (xPackage->isBundle() &&
                           xPackageType->getMediaType().matchAsciiL(
                               RTL_CONSTASCII_STRINGPARAM(
                                   "application/"
                                   "vnd.sun.star.legacy-package-bundle") ));
            }
        }
        abort = !approve;

        // select:
        Sequence< Reference<task::XInteractionContinuation> > conts(
            xRequest->getContinuations() );
        Reference<task::XInteractionContinuation> const * pConts =
            conts.getConstArray();
        sal_Int32 len = conts.getLength();
        for ( sal_Int32 pos = 0; pos < len; ++pos )
        {
            if (approve) {
                Reference<task::XInteractionApprove> xInteractionApprove(
                    pConts[ pos ], UNO_QUERY );
                if (xInteractionApprove.is()) {
                    xInteractionApprove->select();
                    // don't query again for ongoing continuations:
                    approve = false;
                }
            }
            else if (abort) {
                Reference<task::XInteractionAbort> xInteractionAbort(
                    pConts[ pos ], UNO_QUERY );
                if (xInteractionAbort.is()) {
                    xInteractionAbort->select();
                    // don't query again for ongoing continuations:
                    abort = false;
                }
            }
        }
    }
    else {
        // forward to UUI handler:
        if (! m_xHandler.is()) {
            // late init:
            Sequence<Any> handlerArgs( 1 );
            handlerArgs[ 0 ] <<= beans::PropertyValue(
                OUSTR("Context"), -1, Any(m_title),
                beans::PropertyState_DIRECT_VALUE );
            Reference<XComponentContext> const & xContext =
                m_mainDialog->m_xComponentContext;
            m_xHandler.set( xContext->getServiceManager()
                            ->createInstanceWithArgumentsAndContext(
                                OUSTR("com.sun.star.uui.InteractionHandler"),
                                handlerArgs, xContext ), UNO_QUERY_THROW );
        }
        m_xHandler->handle( xRequest );
    }
}

// XProgressHandler
//______________________________________________________________________________
void ProgressCommandEnv::push( Any const & Status )
    throw (RuntimeException)
{
    update_( Status );
}

//______________________________________________________________________________
void ProgressCommandEnv::update_( Any const & Status )
    throw (RuntimeException)
{
    OUString text;
    if (Status.hasValue() && !(Status >>= text)) {
        if (Status.getValueTypeClass() == TypeClass_EXCEPTION)
            text = static_cast<Exception const *>(Status.getValue())->Message;
        if (text.getLength() == 0)
            text = ::comphelper::anyToString(Status); // fallback
        m_mainDialog->errbox( text );
    }
    updateProgress( text );
    ++m_currentInnerProgress;
}

//______________________________________________________________________________
void ProgressCommandEnv::update( Any const & Status )
    throw (RuntimeException)
{
    update_( Status );
}

//______________________________________________________________________________
void ProgressCommandEnv::pop() throw (RuntimeException)
{
    update_( Any() ); // no message
}

}

