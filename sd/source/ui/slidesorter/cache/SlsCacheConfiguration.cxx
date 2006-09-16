/*************************************************************************
 *
 *  $RCSfile: SlsCacheConfiguration.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 19:04:04 $
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
#include "precompiled_sd.hxx"

#include "SlsCacheConfiguration.hxx"
#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>

#include <comphelper/processfactory.hxx>
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XHIERARCHICALNAMEACCESS_HPP_
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace sd { namespace slidesorter { namespace cache {

::boost::shared_ptr<CacheConfiguration> CacheConfiguration::mpInstance;
::boost::weak_ptr<CacheConfiguration> CacheConfiguration::mpWeakInstance;
Timer CacheConfiguration::maReleaseTimer;



::boost::shared_ptr<CacheConfiguration> CacheConfiguration::Instance (void)
{
    ::vos::OGuard aSolarGuard (Application::GetSolarMutex());
    if (mpInstance.get() == NULL)
    {
        // Maybe somebody else kept a previously created instance alive.
        if ( ! mpWeakInstance.expired())
            mpInstance = ::boost::shared_ptr<CacheConfiguration>(mpWeakInstance);
        if (mpInstance.get() == NULL)
        {
            // We have to create a new instance.
            mpInstance.reset(new CacheConfiguration());
            mpWeakInstance = mpInstance;
            // Prepare to release this instance in the near future.
            maReleaseTimer.SetTimeoutHdl(
                LINK(mpInstance.get(),CacheConfiguration,TimerCallback));
            maReleaseTimer.SetTimeout(5000 /* 5s */);
            maReleaseTimer.Start();
        }
    }
    return mpInstance;
}




CacheConfiguration::CacheConfiguration (void)
{
    // Get the cache size from configuration.
    const ::rtl::OUString sConfigurationProviderServiceName(
        RTL_CONSTASCII_USTRINGPARAM(
            "com.sun.star.configuration.ConfigurationProvider"));
    const ::rtl::OUString sPathToImpressConfigurationRoot(
        RTL_CONSTASCII_USTRINGPARAM("/org.openoffice.Office.Impress/"));
    const ::rtl::OUString sPathToNode(
        RTL_CONSTASCII_USTRINGPARAM(
            "MultiPaneGUI/SlideSorter/PreviewCache"));

    try
    {
        do
        {
            // Obtain access to the configuration.
            Reference<lang::XMultiServiceFactory> xProvider (
                ::comphelper::getProcessServiceFactory()->createInstance(
                    sConfigurationProviderServiceName),
                UNO_QUERY);
            if ( ! xProvider.is())
                break;

            // Obtain access to Impress configuration.
            Sequence<Any> aCreationArguments(3);
            aCreationArguments[0] = makeAny(beans::PropertyValue(
                ::rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM("nodepath")),
                0,
                makeAny(sPathToImpressConfigurationRoot),
                beans::PropertyState_DIRECT_VALUE));
            aCreationArguments[1] = makeAny(beans::PropertyValue(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("depth")),
                0,
                makeAny((sal_Int32)-1),
                beans::PropertyState_DIRECT_VALUE));
            aCreationArguments[2] = makeAny(beans::PropertyValue(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("lazywrite")),
                0,
                makeAny(true),
                beans::PropertyState_DIRECT_VALUE));
            ::rtl::OUString sAccessService (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                "com.sun.star.configuration.ConfigurationAccess")));
            Reference<XInterface> xRoot (xProvider->createInstanceWithArguments(
                sAccessService, aCreationArguments));
            if ( ! xRoot.is())
                break;
            Reference<container::XHierarchicalNameAccess> xHierarchy (xRoot, UNO_QUERY);
            if ( ! xHierarchy.is())
                break;

            // Get the node for the slide sorter preview cache.
            mxCacheNode = Reference<container::XNameAccess>(
                xHierarchy->getByHierarchicalName(sPathToNode),
                UNO_QUERY);
        }
        while (false);
    }
    catch (RuntimeException aException)
    {
        (void)aException;
    }
    catch (Exception aException)
    {
        (void)aException;
    }
}




Any CacheConfiguration::GetValue (const ::rtl::OUString& rName)
{
    Any aResult;

    if (mxCacheNode != NULL)
    {
        try
        {
            aResult = mxCacheNode->getByName(rName);
        }
        catch (Exception aException)
        {
            (void)aException;
        }
    }

    return aResult;
}




IMPL_LINK(CacheConfiguration,TimerCallback, Timer*,pTimer)
{
    // Release out reference to the instance.
    mpInstance.reset();
    return 0;
}


} } } // end of namespace ::sd::slidesorter::cache
