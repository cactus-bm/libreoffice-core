/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: updatecheckjob.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-06 14:37:37 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_extensions.hxx"

#include "updatecheck.hxx"
#include "updatecheckconfig.hxx"
#include "updatehdl.hxx"

#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implementationentry.hxx>

#include <com/sun/star/task/XJob.hpp>

namespace beans = com::sun::star::beans ;
namespace lang = com::sun::star::lang ;
namespace task = com::sun::star::task ;
namespace uno = com::sun::star::uno ;

#define UNISTRING(s) rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(s))

namespace
{

class UpdateCheckJob :
    public ::cppu::WeakImplHelper2< task::XJob, lang::XServiceInfo >
{
    virtual ~UpdateCheckJob();

public:

    UpdateCheckJob(const uno::Reference<uno::XComponentContext>& xContext);

    static uno::Sequence< rtl::OUString > getServiceNames();
    static rtl::OUString getImplName();

    // Allows runtime exceptions to be thrown by const methods
    inline SAL_CALL operator uno::Reference< uno::XInterface > () const
        { return const_cast< cppu::OWeakObject * > (static_cast< cppu::OWeakObject const * > (this)); };

    // XJob
    virtual uno::Any SAL_CALL execute(const uno::Sequence<beans::NamedValue>&)
        throw (lang::IllegalArgumentException, uno::Exception);

    // XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName()
        throw (uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService(rtl::OUString const & serviceName)
        throw (uno::RuntimeException);
    virtual uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames()
        throw (uno::RuntimeException);

private:
    uno::Reference<uno::XComponentContext> m_xContext;
};

//------------------------------------------------------------------------------

UpdateCheckJob::UpdateCheckJob(const uno::Reference<uno::XComponentContext>& xContext) :
    m_xContext(xContext)
{
}

//------------------------------------------------------------------------------

UpdateCheckJob::~UpdateCheckJob()
{
}

//------------------------------------------------------------------------------

uno::Sequence< rtl::OUString >
UpdateCheckJob::getServiceNames()
{
    uno::Sequence< rtl::OUString > aServiceList(1);
    aServiceList[0] = UNISTRING( "com.sun.star.setup.UpdateCheck");
    return aServiceList;
};

//------------------------------------------------------------------------------

rtl::OUString
UpdateCheckJob::getImplName()
{
    return UNISTRING( "vnd.sun.UpdateCheck");
}


//------------------------------------------------------------------------------

uno::Any
UpdateCheckJob::execute(const uno::Sequence<beans::NamedValue>& namedValues)
    throw (lang::IllegalArgumentException, uno::Exception)
{
    uno::Sequence<beans::NamedValue> aConfig =
        getValue< uno::Sequence<beans::NamedValue> > (namedValues, "JobConfig");

    rtl::Reference<UpdateCheck> aController(UpdateCheck::get());
    aController->initialize(aConfig, m_xContext);

    /* Determine the way we got invoked here -
     * see Developers Guide Chapter "4.7.2 Jobs" to understand the magic
     */

    uno::Sequence<beans::NamedValue> aEnvironment =
        getValue< uno::Sequence<beans::NamedValue> > (namedValues, "Environment");

    rtl::OUString aEventName = getValue< rtl::OUString > (aEnvironment, "EventName");

    if( ! aEventName.equalsAscii("onFirstVisibleTask") )
    {
        aController->showDialog(true);
    }

    return uno::Any();
}

//------------------------------------------------------------------------------

rtl::OUString SAL_CALL
UpdateCheckJob::getImplementationName() throw (uno::RuntimeException)
{
    return getImplName();
}

//------------------------------------------------------------------------------

uno::Sequence< rtl::OUString > SAL_CALL
UpdateCheckJob::getSupportedServiceNames() throw (uno::RuntimeException)
{
    return getServiceNames();
}

//------------------------------------------------------------------------------

sal_Bool SAL_CALL
UpdateCheckJob::supportsService( rtl::OUString const & serviceName ) throw (uno::RuntimeException)
{
    uno::Sequence< rtl::OUString > aServiceNameList = getServiceNames();

    for( sal_Int32 n=0; n < aServiceNameList.getLength(); n++ )
        if( aServiceNameList[n].equals(serviceName) )
            return sal_True;

    return sal_False;
}

} // anonymous namespace

//------------------------------------------------------------------------------

static uno::Reference<uno::XInterface> SAL_CALL
createJobInstance(const uno::Reference<uno::XComponentContext>& xContext)
{
    return *new UpdateCheckJob(xContext);
}

//------------------------------------------------------------------------------

static uno::Reference<uno::XInterface> SAL_CALL
createConfigInstance(const uno::Reference<uno::XComponentContext>& xContext)
{
    return *UpdateCheckConfig::get(xContext, *UpdateCheck::get());
}

//------------------------------------------------------------------------------

static const cppu::ImplementationEntry kImplementations_entries[] =
{
    {
        createJobInstance,
        UpdateCheckJob::getImplName,
        UpdateCheckJob::getServiceNames,
        cppu::createSingleComponentFactory,
        NULL,
        0
    },
    {
        createConfigInstance,
        UpdateCheckConfig::getImplName,
        UpdateCheckConfig::getServiceNames,
        cppu::createSingleComponentFactory,
        NULL,
        0
    },
    { NULL, NULL, NULL, NULL, NULL, 0 }
} ;

//------------------------------------------------------------------------------

extern "C" void SAL_CALL
component_getImplementationEnvironment( const sal_Char **aEnvTypeName, uno_Environment **)
{
    *aEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME ;
}

//------------------------------------------------------------------------------

extern "C" sal_Bool SAL_CALL
component_writeInfo(void *pServiceManager, void *pRegistryKey)
{
    return cppu::component_writeInfoHelper(
        pServiceManager,
        pRegistryKey,
        kImplementations_entries
    );
}

//------------------------------------------------------------------------------

extern "C" void *
component_getFactory(const sal_Char *pszImplementationName, void *pServiceManager, void *pRegistryKey)
{
    return cppu::component_getFactoryHelper(
        pszImplementationName,
        pServiceManager,
        pRegistryKey,
        kImplementations_entries) ;
}
