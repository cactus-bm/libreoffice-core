/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: jobexecutor.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:23:05 $
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

#ifndef __FRAMEWORK_JOBS_JOBEXECUTOR_HXX_
#define __FRAMEWORK_JOBS_JOBEXECUTOR_HXX_

//_______________________________________
// my own includes

#ifndef __FRAMEWORK_CONFIG_CONFIGACCESS_HXX_
#include <jobs/configaccess.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
#include <threadhelp/threadhelpbase.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_XINTERFACE_HXX_
#include <macros/xinterface.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_XTYPEPROVIDER_HXX_
#include <macros/xtypeprovider.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_XSERVICEINFO_HXX_
#include <macros/xserviceinfo.hxx>
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

#ifndef _COM_SUN_STAR_TASK_XJOBEXECUTOR_HPP_
#include <com/sun/star/task/XJobExecutor.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XCONTAINERLISTENER_HPP_
#include <com/sun/star/container/XContainerListener.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XEVENTLISTENER_HPP_
#include <com/sun/star/lang/XEventListener.hpp>
#endif

#ifndef _COM_SUN_STAR_DOCUMENT_XEVENTLISTENER_HPP_
#include <com/sun/star/document/XEventListener.hpp>
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
/**
    @short  implements a job executor, which can be triggered from any code
    @descr  It uses the given trigger event to locate any registered job service
            inside the configuration and execute it. Of course it controls the
            liftime of such jobs too.
 */
class JobExecutor : public  css::lang::XTypeProvider
                  , public  css::lang::XServiceInfo
                  , public  css::task::XJobExecutor
                  , public  css::container::XContainerListener // => lang.XEventListener
                  , public  css::document::XEventListener
                  , private ThreadHelpBase
                  , public  ::cppu::OWeakObject
{
    //___________________________________
    // member

    private:

        /** reference to the uno service manager */
        css::uno::Reference< css::lang::XMultiServiceFactory > m_xSMGR;

        /** cached list of all registered event names of cfg for call optimization. */
        OUStringList m_lEvents;

        /** we listen at the configuration for changes at the event list. */
        ConfigAccess m_aConfig;

    //___________________________________
    // native interface methods

    public:

                  JobExecutor( const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR );
         virtual ~JobExecutor(                                                                     );

    //___________________________________
    // uno interface methods

    public:

        // XInterface, XTypeProvider, XServiceInfo
        DECLARE_XINTERFACE
        DECLARE_XTYPEPROVIDER
        DECLARE_XSERVICEINFO

        // task.XJobExecutor
        virtual void SAL_CALL trigger( const ::rtl::OUString& sEvent ) throw(css::uno::RuntimeException);

        // document.XEventListener
        virtual void SAL_CALL notifyEvent( const css::document::EventObject& aEvent ) throw(css::uno::RuntimeException);

        // container.XContainerListener
        virtual void SAL_CALL elementInserted( const css::container::ContainerEvent& aEvent ) throw(css::uno::RuntimeException);
        virtual void SAL_CALL elementRemoved ( const css::container::ContainerEvent& aEvent ) throw(css::uno::RuntimeException);
        virtual void SAL_CALL elementReplaced( const css::container::ContainerEvent& aEvent ) throw(css::uno::RuntimeException);

        // lang.XEventListener
        virtual void SAL_CALL disposing( const css::lang::EventObject& aEvent ) throw(css::uno::RuntimeException);
};

} // namespace framework

#endif // __FRAMEWORK_JOBS_JOBEXECUTOR_HXX_
