/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: officeipcthread.hxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 17:12:08 $
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

#ifndef _DESKTOP_OFFICEIPCTHREAD_HXX_
#define _DESKTOP_OFFICEIPCTHREAD_HXX_

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XTERMINATELISTENER_HPP_
#include <com/sun/star/frame/XTerminateListener.hpp>
#endif

#ifndef _VOS_PIPE_HXX_
#include <vos/pipe.hxx>
#endif
#ifndef _VOS_SECURITY_HXX_
#include <vos/security.hxx>
#endif
#ifndef _VOS_THREAD_HXX_
#include <vos/thread.hxx>
#endif
#ifndef _VOS_SIGNAL_HXX_
#include <vos/signal.hxx>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _CPPUHELPER_WEAKBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif
#ifndef _OSL_CONDITN_HXX_
#include <osl/conditn.hxx>
#endif

namespace desktop
{

class SalMainPipeExchangeSignalHandler : public vos::OSignalHandler
{
    virtual TSignalAction SAL_CALL signal(TSignalInfo *pInfo);
};

// A request for the current office
// that was given by command line or by IPC pipe communication.
struct ProcessDocumentsRequest
{
    ::rtl::OUString aModule;
    ::rtl::OUString aOpenList;      // Documents that should be opened in the default way
    ::rtl::OUString aViewList;      // Documents that should be opened in viewmode
    ::rtl::OUString aStartList;     // Documents/Presentations that should be started
    ::rtl::OUString aPrintList;     // Documents that should be printed on default printer
    ::rtl::OUString aForceOpenList; // Documents that should be forced to open for editing (even templates)
    ::rtl::OUString aForceNewList;  // Documents that should be forced to create a new document
    ::rtl::OUString aPrinterName;   // The printer name that should be used for printing
    ::rtl::OUString aPrintToList;   // Documents that should be printed on the given printer
    ::osl::Condition *pcProcessed;  // pointer condition to be set when the request has been processed
};

class DispatchWatcher;
class OfficeIPCThread : public vos::OThread
{
  private:
    static OfficeIPCThread*     pGlobalOfficeIPCThread;
    static ::osl::Mutex*        pOfficeIPCThreadMutex;

    vos::OPipe                  maPipe;
    vos::OStreamPipe            maStreamPipe;
    rtl::OUString               maPipeIdent;
    sal_Bool                    mbBlockRequests;
    int                         mnPendingRequests;
    DispatchWatcher*            mpDispatchWatcher;

    /* condition to be set when the request has been processed */
    ::osl::Condition cProcessed;

    /* condition to be set when the main event loop is ready
       otherwise an error dialogs event loop could eat away
       requests from a 2nd office */
    ::osl::Condition cReady;

    static ::osl::Mutex&        GetMutex();
    static const char *sc_aTerminationSequence;
    static const int sc_nTSeqLength;
    static const char *sc_aShowSequence;
    static const int sc_nShSeqLength;
    static const char *sc_aConfirmationSequence;
    static const int sc_nCSeqLength;

    OfficeIPCThread();

  protected:
    /// Working method which should be overridden
    virtual void SAL_CALL run();

  public:
    enum Status
    {
        IPC_STATUS_OK,
        IPC_STATUS_2ND_OFFICE,
        IPC_STATUS_BOOTSTRAP_ERROR
    };

    virtual ~OfficeIPCThread();

    // controlling pipe communication during shutdown
    static OfficeIPCThread*     GetOfficeIPCThread();
    static void                 BlockAllRequests();
    static sal_Bool             AreRequestsPending();
    static void                 RequestsCompleted( int n = 1 );
    static void                 ExecuteCmdLineRequests( ProcessDocumentsRequest& );

    // return FALSE if second office
    static Status               EnableOfficeIPCThread();
    static void                 DisableOfficeIPCThread();
    // start dispatching events...
    static void                 SetReady(OfficeIPCThread* pThread = NULL);
};


class OfficeIPCThreadController : public ::cppu::WeakImplHelper2<
                                            ::com::sun::star::lang::XServiceInfo,
                                            ::com::sun::star::frame::XTerminateListener >
{
    public:
        OfficeIPCThreadController() {}
        virtual ~OfficeIPCThreadController() {}

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName()
            throw ( ::com::sun::star::uno::RuntimeException );
        virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName )
            throw ( ::com::sun::star::uno::RuntimeException );
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames()
            throw ( ::com::sun::star::uno::RuntimeException );

        // XEventListener
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source )
            throw( ::com::sun::star::uno::RuntimeException );

        // XTerminateListener
        virtual void SAL_CALL queryTermination( const ::com::sun::star::lang::EventObject& aEvent )
            throw( ::com::sun::star::frame::TerminationVetoException, ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL notifyTermination( const ::com::sun::star::lang::EventObject& aEvent )
            throw( ::com::sun::star::uno::RuntimeException );
};

}

#endif // _DESKTOP_OFFICEIPCTHREAD_HXX_
