/*************************************************************************
 *
 *  $RCSfile: jobdata.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: vg $ $Date: 2003-05-22 08:35:32 $
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

#ifndef __FRAMEWORK_JOBS_JOBDATA_HXX_
#define __FRAMEWORK_JOBS_JOBDATA_HXX_

//_______________________________________
// my own includes

#ifndef __FRAMEWORK_CONFIG_CONFIGACCESS_HXX_
#include <jobs/configaccess.hxx>
#endif

#ifndef __FRAMEWORK_JOBS_JOBRESULT_HXX_
#include <jobs/jobresult.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
#include <threadhelp/threadhelpbase.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_DEBUG_HXX_
#include <macros/debug.hxx>
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

#ifndef _COM_SUN_STAR_BEANS_NAMEDVALUE_HPP_
#include <com/sun/star/beans/NamedValue.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_DISPATCHRESULTEVENT_HPP_
#include <com/sun/star/frame/DispatchResultEvent.hpp>
#endif

//_______________________________________
// other includes

#ifndef _DATETIME_HXX
#include <tools/datetime.hxx>
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
    @short  holds all neccessary informations about a job and
            handle it's configuration (if any exist!)
    @descr  It can be used rom different use cases as a container
            (or proxy) for all config data of a registered job.
            But it doesn't implement any execute functionality!
 */
class JobData : private ThreadHelpBase
{
    //___________________________________
    // const

    public:

        /// specifies the root package and key to find registered jobs
        static const sal_Char* JOBCFG_ROOT;
        /// define the cfg key "Arguments" of a job relativ to JOBCFG_ROOT/<job alias>
        static const sal_Char* JOBCFG_PROP_ARGUMENTS;
        /// define the cfg key "Service" of a job relativ to JOBCFG_ROOT/<job alias>
        static const sal_Char* JOBCFG_PROP_SERVICE;

        /// specifies the root package and key to find event registrations
        static const sal_Char* EVENTCFG_ROOT;
        /// define the cfg key "JobList" of an event relativ to EVENTCFG_ROOT/<event>
        static const sal_Char* EVENTCFG_PATH_JOBLIST;
        /// define the cfg key "AdminTime" of a job registration relativ to EVENTCFG_ROOT/<event>/EVENTCFG_PROP_JOBLIST/<job alias>
        static const sal_Char* EVENTCFG_PROP_ADMINTIME;
        /// define the cfg key "UserTime" of a job registration relativ to EVENTCFG_ROOT/<event>/EVENTCFG_PROP_JOBLIST/<job alias>
        static const sal_Char* EVENTCFG_PROP_USERTIME;

        /// mark the starting point of static job data inside argument list of job execution
        static const sal_Char* PROPSET_CONFIG;
        /// mark the starting point of job specific data inside argument list of job execution
        static const sal_Char* PROPSET_OWNCONFIG;
        /// mark the starting point of environment data inside argument list of job execution
        static const sal_Char* PROPSET_ENVIRONMENT;
        /// mark the starting point of any other dynamic generated data inside argument list of job execution (e.g. from a dispatch() request)
        static const sal_Char* PROPSET_DYNAMICDATA;

        static const sal_Char* PROP_ALIAS;
        static const sal_Char* PROP_EVENTNAME;
        static const sal_Char* PROP_ENVTYPE;
        static const sal_Char* PROP_FRAME;
        static const sal_Char* PROP_MODEL;
        static const sal_Char* PROP_SERVICE;

    //___________________________________
    // structs

    public:

        /** These values can be used to differe between jobs with and jobs without
            a configuration. Of course an "unknown state" should be available too,
            to detect a missing initialization.
         */
        enum EMode
        {
            /// indicates a missing initialization
            E_UNKNOWN_MODE,
            /// indicates a job with configuration (They alias represent the config key name.)
            E_ALIAS,
            /// indicates a job without configuration (The pure UNO implementation is used only.)
            E_SERVICE,
            /// indicates a job with configuration, which was triggered by an event
            E_EVENT
        };

        /** These values represent the environment type, in which a job can run.
            A job must known, from which binding it will be started. Because
            it's initialization data depends from that!
         */
        enum EEnvironment
        {
            /// indicates a missing initialization
            E_UNKNOWN_ENVIRONMENT,
            /// this job is used by the global JobExecutor service
            E_EXECUTION,
            /// this job is used by the global dispatch framework
            E_DISPATCH,
            /// this job is used by the global event broadcaster
            E_DOCUMENTEVENT
        };

    //___________________________________
    // member

    private:

        /**
            reference to the uno service manager.
            We need it for creating of own uno services ... e.g. for
            opening the configuration.
         */
        css::uno::Reference< css::lang::XMultiServiceFactory > m_xSMGR;

        /**
            An instance of this class can be used in two different modes:
                - as a configured job
                - as a job without any configuration
            First mode is triggered by an alias, which points to the
            configuration entries. Second mode is specified by an uno service
            or implementation name. Then we does the same things (use the same interfaces)
            but don't handle any configuration data.
            The effect: This mode can be detected by this member.
         */
        EMode m_eMode;

        /**
            Because jobs can be bind to different mechanism inside office, a job
            should know inside which environment it runs. E.g. a job can be executed
            by the global JobExecutor service (triggered by an event) or e.g. as part
            of the global dispatch framework (triggered by an UI control e.g. a menu entry).
         */
        EEnvironment m_eEnvironment;

        /**
            the alias name of this job.
            Is used as entry of configuration set for job registration, to find all
            neccessary properties of it..
         */
        ::rtl::OUString m_sAlias;

        /**
            the uno implementation name of this job.
            It's readed from the configuration. Don't set it from outside!
         */
        ::rtl::OUString m_sService;

        /**
            a job can be registered for an event.
            It can be an empty value! But it will be set from outside any times.
            Because it's not clear which job this instance should represent if an event
            (instaed of an alias) comes in. Because there can be multiple registrations
            for this event. We use this information only, to merge it with the job specific
            arguments. A job can be called so, with a) it's onw config data and b) some dynamic
            environment data.
         */
        ::rtl::OUString m_sEvent;

        /**
            job specific configuration items ... unknown for us!
            It's readed from the configuration. Don't set it from outside!
         */
        css::uno::Sequence< css::beans::NamedValue > m_lArguments;

        /**
            after a job was sucessfully executed (by any outside code using our
            informations) it can return a result. This member make it part of this
            container too. So it can be used for further things.
            We use it also to actualize our internal state and the configuration
            of the job. But note: only the last result will be saved here!
         */
        JobResult m_aLastExecutionResult;

    //___________________________________
    // native interface

    public:

                 JobData( const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR );
                 JobData( const JobData&                                                rCopy );
        virtual ~JobData(                                                                     );

        void operator=( const JobData& rCopy );

        EMode                                        getMode                 () const;
        EEnvironment                                 getEnvironment          () const;
        ::rtl::OUString                              getEnvironmentDescriptor() const;
        ::rtl::OUString                              getAlias                () const;
        ::rtl::OUString                              getService              () const;
        ::rtl::OUString                              getEvent                () const;
        css::uno::Sequence< css::beans::NamedValue > getConfig               () const;
        css::uno::Sequence< css::beans::NamedValue > getJobConfig            () const;
        JobResult                                    getResult               () const;

        sal_Bool                                     hasConfig               () const;

        void                                         setEnvironment (       EEnvironment                                  eEnvironment );
        void                                         setAlias       ( const ::rtl::OUString&                              sAlias       );
        void                                         setService     ( const ::rtl::OUString&                              sService     );
        void                                         setEvent       ( const ::rtl::OUString&                              sEvent       ,
                                                                      const ::rtl::OUString&                              sAlias       );
        void                                         setJobConfig   ( const css::uno::Sequence< css::beans::NamedValue >& lArguments   );
        void                                         setResult      ( const JobResult&                                    aResult      );
        void                                         disableJob     (                                                                  );

        static css::uno::Sequence< ::rtl::OUString > getEnabledJobsForEvent( const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR  ,
                                                                             const ::rtl::OUString&                                        sEvent );

    //___________________________________
    // private helper

    private:

        void impl_reset();
};

} // namespace framework

#endif // __FRAMEWORK_JOBS_JOBDATA_HXX_
