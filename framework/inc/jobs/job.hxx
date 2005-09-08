/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: job.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:22:20 $
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

#ifndef __FRAMEWORK_JOBS_JOB_HXX_
#define __FRAMEWORK_JOBS_JOB_HXX_

//_______________________________________
// my own includes

#ifndef __FRAMEWORK_JOBS_JOBRESULT_HXX_
#include <jobs/jobresult.hxx>
#endif

#ifndef __FRAMEWORK_JOBS_JOBDATA_HXX_
#include <jobs/jobdata.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
#include <threadhelp/threadhelpbase.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_DEBUG_HXX_
#include <macros/debug.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_XINTERFACE_HXX_
#include <macros/xinterface.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_XTYPEPROVIDER_HXX_
#include <macros/xtypeprovider.hxx>
#endif

#ifndef __FRAMEWORK_STDTYPES_H_
#include <stdtypes.h>
#endif

#ifndef __FRAMEWORK_GENERAL_H_
#include <general.h>
#endif

//_______________________________________
// interface includes

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XTYPEPROVIDER_HPP_
#include <com/sun/star/lang/XTypeProvider.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XDESKTOP_HPP_
#include <com/sun/star/frame/XDesktop.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XDISPATCHRESULTLISTENER_HPP_
#include <com/sun/star/frame/XDispatchResultListener.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XTERMINATELISTENER_HPP_
#include <com/sun/star/frame/XtERMINATEListener.hpp>
#endif

#ifndef _COM_SUN_STAR_TASK_XJOBLISTENER_HPP_
#include <com/sun/star/task/XJobListener.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_XCLOSELISTENER_HPP_
#include <com/sun/star/util/XCloseListener.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_DISPATCHRESULTEVENT_HPP_
#include <com/sun/star/frame/DispatchResultEvent.hpp>
#endif

//_______________________________________
// other includes

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

//_______________________________________
// namespace

namespace framework{

//_______________________________________
// public const

//_______________________________________
// definitions

/**
    @short  it represent a job; execute it and control it's lifetime

    @descr  This implemetation can be used to wrapp jobs, execute it
            synchronously or asynchronous, control it's lifetime
            and differe between jobs with and without configuration.
 */
class Job : public  css::lang::XTypeProvider
          , public  css::task::XJobListener
          , public  css::frame::XTerminateListener
          , public  css::util::XCloseListener
          , private ThreadHelpBase
          , public  ::cppu::OWeakObject
{
    //___________________________________
    // structs

    private:

    /** different possible states for the internal wrapped job.
        It can be started, stopped by a queryClosing() request or
        disposed() by a notifyClosing() request ...
     */
    enum ERunState
    {
        E_NEW,
        E_RUNNING,
        E_STOPPED_OR_FINISHED,
        E_DISPOSED
    };

    //___________________________________
    // member

    private:

        /**
            hold all neccessary informations about this job.
            It can be used for both modes: with and without configuration.
         */
        JobData m_aJobCfg;

        /**
            We need it to create own services on demand.
         */
        css::uno::Reference< css::lang::XMultiServiceFactory > m_xSMGR;

        /**
            Hold the (may asynchronous) job alive.
         */
        css::uno::Reference< css::uno::XInterface > m_xJob;

        /**
            Used to wait for finishing of asynchronous started jobs.
         */
        ::osl::Condition m_aAsyncWait;

        /**
            For some special cases we must know the environment, in which
            this job runs. Means the frame inside which we may was triggered.
            We use it too, to listen for closing events of this ressource.

            Please note: If m_xFrame is set - m_xModel should be NULL.
            Only one environment can be supported realy.
         */
        css::uno::Reference< css::frame::XFrame > m_xFrame;

        /**
            For some special cases we must know the environment, in which
            this job runs. Means the document inside which we may was triggered.
            We use it too, to listen for closing events of this ressource.

            Please note: If m_xModel is set - m_xFrame should be NULL.
            Only one environment can be supported realy.
         */
        css::uno::Reference< css::frame::XModel > m_xModel;

        /**
            We are registered at this instance to listen for office shutdown events.
            It's neccessary supress it (if possible) or to react in the right way.
         */
        css::uno::Reference< css::frame::XDesktop > m_xDesktop;

        /**
            A job can return a dispatch result event after finishing its work.
            We have to transport it to any outside interested listener then.
            (see m_xResultSourceFake for further informations too!)
         */
        css::uno::Reference< css::frame::XDispatchResultListener > m_xResultListener;

        /**
            We can't set ourself as source of a dispatch result event ... nor our job.
            Because the listener (set as m_xResultListener) expect the original instance,
            where it was registered. This original instance is the user of this class.
            It must be set explicitly and will be used to fake the source of the event!
         */
        css::uno::Reference< css::uno::XInterface > m_xResultSourceFake;

        /**
            Holds the state, if we are listen for desktop/frame or model closing events or not.
            The used references are not realy enough to detect a valid listener connection.
            Thats why we use this additional information here too.
         */
        sal_Bool m_bListenOnDesktop;
        sal_Bool m_bListenOnFrame;
        sal_Bool m_bListenOnModel;

        /**
            In case we got a close request from our desktop/frame/model (on which we listen) ... and
            the ownership was delivered there ... we have to close ourself and this object
            in case the internal wrapped and running job finish his work.
         */
        sal_Bool m_bPendingCloseFrame;
        sal_Bool m_bPendingCloseModel;

        /**
            indicates in which state the internal job currently exist.

            We can use this information to throw any suitable veto exception
            to prevent the environment against dieing or supress superflous dispose()
            calls at the job.
         */
        ERunState m_eRunState;

    //___________________________________
    // native interface

    public:

                 Job( const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR  ,
                      const css::uno::Reference< css::frame::XFrame >&              xFrame );
                 Job( const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR  ,
                      const css::uno::Reference< css::frame::XModel >&              xModel );
        virtual ~Job(                                                                      );

        void     setDispatchResultFake( const css::uno::Reference< css::frame::XDispatchResultListener >& xListener    ,
                                        const css::uno::Reference< css::uno::XInterface >&                xSourceFake  );
        void     setJobData           ( const JobData&                                                    aData        );
        JobData  getJobData           (                                                                                );
        JobData& getJobDataRef        (                                                                                );
        void     execute              ( const css::uno::Sequence< css::beans::NamedValue >&               lDynamicArgs );
        void     die                  (                                                                                );

    private:

        css::uno::Sequence< css::beans::NamedValue > impl_generateJobArgs  ( const css::uno::Sequence< css::beans::NamedValue >& lDynamicArgs );
        void                                         impl_reactForJobResult( const css::uno::Any&                                aResult      );
        void                                         impl_startListening   (                                                                  );
        void                                         impl_stopListening    (                                                                  );

    //___________________________________
    // uno interface

    public:

        DECLARE_XINTERFACE
        DECLARE_XTYPEPROVIDER

        // XJobListener
        virtual void SAL_CALL jobFinished( const css::uno::Reference< css::task::XAsyncJob >& xJob,
                                           const css::uno::Any&                               aResult ) throw(css::uno::RuntimeException);

        // XTerminateListener
        virtual void SAL_CALL queryTermination ( const css::lang::EventObject& aEvent ) throw(css::frame::TerminationVetoException,
                                                                                              css::uno::RuntimeException          );
        virtual void SAL_CALL notifyTermination( const css::lang::EventObject& aEvent ) throw(css::uno::RuntimeException          );

        // XCloseListener
        virtual void SAL_CALL queryClosing ( const css::lang::EventObject& aEvent         ,
                                                   sal_Bool                bGetsOwnership ) throw(css::util::CloseVetoException,
                                                                                                  css::uno::RuntimeException   );
        virtual void SAL_CALL notifyClosing( const css::lang::EventObject& aEvent         ) throw(css::uno::RuntimeException   );

        // XEventListener
        virtual void SAL_CALL disposing( const css::lang::EventObject& aEvent ) throw(css::uno::RuntimeException);
};

} // namespace framework

#endif // __FRAMEWORK_JOBS_JOB_HXX_
