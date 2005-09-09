/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: configureucb.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 16:36:45 $
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

#ifndef _UCBHELPER_CONFIGUREUCB_HXX_
#include <ucbhelper/configureucb.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENTPROVIDER_HPP_
#include <com/sun/star/ucb/XContentProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENTPROVIDERMANAGER_HPP_
#include <com/sun/star/ucb/XContentProviderManager.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HPP_
#include <com/sun/star/uno/RuntimeException.hpp>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#ifndef _VOS_DIAGNOSE_H_
#include <vos/diagnose.hxx>
#endif

#ifndef _UCBHELPER_PROVCONF_HXX_
#include <provconf.hxx>
#endif
#ifndef _UCBHELPER_REGISTERUCB_HXX_
#include <registerucb.hxx>
#endif

namespace unnamed_ucbhelper_configureucb {}
using namespace unnamed_ucbhelper_configureucb;
    // unnamed namespaces don't work well yet...

using namespace com::sun;
using namespace com::sun::star;

namespace unnamed_ucbhelper_configureucb {

bool fillPlaceholders(rtl::OUString const & rInput,
                      uno::Sequence< uno::Any > const & rReplacements,
                      rtl::OUString * pOutput)
{
    sal_Unicode const * p = rInput.getStr();
    sal_Unicode const * pEnd = p + rInput.getLength();
    sal_Unicode const * pCopy = p;
    rtl::OUStringBuffer aBuffer;
    while (p != pEnd)
        switch (*p++)
        {
            case '&':
                if (pEnd - p >= 4
                    && p[0] == 'a' && p[1] == 'm' && p[2] == 'p'
                    && p[3] == ';')
                {
                    aBuffer.append(pCopy, p - 1 - pCopy);
                    aBuffer.append(sal_Unicode('&'));
                    p += 4;
                    pCopy = p;
                }
                else if (pEnd - p >= 3
                         && p[0] == 'l' && p[1] == 't' && p[2] == ';')
                {
                    aBuffer.append(pCopy, p - 1 - pCopy);
                    aBuffer.append(sal_Unicode('<'));
                    p += 3;
                    pCopy = p;
                }
                else if (pEnd - p >= 3
                         && p[0] == 'g' && p[1] == 't' && p[2] == ';')
                {
                    aBuffer.append(pCopy, p - 1 - pCopy);
                    aBuffer.append(sal_Unicode('>'));
                    p += 3;
                    pCopy = p;
                }
                break;

            case '<':
                sal_Unicode const * q = p;
                while (q != pEnd && *q != '>')
                    ++q;
                if (q == pEnd)
                    break;
                rtl::OUString aKey(p, q - p);
                rtl::OUString aValue;
                bool bFound = false;
                for (sal_Int32 i = 2; i + 1 < rReplacements.getLength();
                     i += 2)
                {
                    rtl::OUString aReplaceKey;
                    if ((rReplacements[i] >>= aReplaceKey)
                        && aReplaceKey == aKey
                        && (rReplacements[i + 1] >>= aValue))
                    {
                        bFound = true;
                        break;
                    }
                }
                if (!bFound)
                    return false;
                aBuffer.append(pCopy, p - 1 - pCopy);
                aBuffer.append(aValue);
                p = q + 1;
                pCopy = p;
                break;
        }
    aBuffer.append(pCopy, pEnd - pCopy);
    *pOutput = aBuffer.makeStringAndClear();
    return true;
}

}

namespace ucb {

//============================================================================
//
//  configureUcb
//
//============================================================================

bool
configureUcb(
    uno::Reference< star::ucb::XContentProviderManager > const & rManager,
    uno::Reference< lang::XMultiServiceFactory > const & rServiceFactory,
    ContentProviderDataList const & rData,
    ContentProviderRegistrationInfoList * pInfos)
    throw (uno::RuntimeException)
{
    ContentProviderDataList::const_iterator aEnd(rData.end());
    for (ContentProviderDataList::const_iterator aIt(rData.begin());
         aIt != aEnd; ++aIt)
    {
        ContentProviderRegistrationInfo aInfo;
        bool bSuccess = registerAtUcb(rManager,
                                      rServiceFactory,
                                      aIt->ServiceName,
                                      aIt->Arguments,
                                      aIt->URLTemplate,
                                      &aInfo);

        if (bSuccess && pInfos)
            pInfos->push_back(aInfo);
    }

    return true;
}

//============================================================================
//
//  configureUcb
//
//============================================================================

bool
configureUcb(
    uno::Reference< star::ucb::XContentProviderManager > const & rManager,
    uno::Reference< lang::XMultiServiceFactory > const & rServiceFactory,
    uno::Sequence< uno::Any > const & rArguments,
    std::vector< ContentProviderRegistrationInfo > * pInfos)
    throw (uno::RuntimeException)
{
    rtl::OUString aKey1;
    rtl::OUString aKey2;
    if (rArguments.getLength() < 2
        || !(rArguments[0] >>= aKey1) || !(rArguments[1] >>= aKey2))
    {
        VOS_ENSURE(false, "ucb::configureUcb(): Bad arguments");
        return false;
    }

    ContentProviderDataList aData;
    if (!getContentProviderData(rServiceFactory, aKey1, aKey2, aData))
    {
        VOS_ENSURE(false, "ucb::configureUcb(): No configuration");
        return false;
    }

    ContentProviderDataList::const_iterator aEnd(aData.end());
    for (ContentProviderDataList::const_iterator aIt(aData.begin());
         aIt != aEnd; ++aIt)
    {
        rtl::OUString aProviderArguments;
        if (fillPlaceholders(aIt->Arguments,
                             rArguments,
                             &aProviderArguments))
        {
            ContentProviderRegistrationInfo aInfo;
            bool bSuccess = registerAtUcb(rManager,
                                          rServiceFactory,
                                          aIt->ServiceName,
                                          aProviderArguments,
                                          aIt->URLTemplate,
                                          &aInfo);
            VOS_ENSURE(bSuccess, "ucb::configureUcb(): Bad content provider");

            if (bSuccess && pInfos)
                pInfos->push_back(aInfo);
        }
        else
            VOS_ENSURE(false,
                       "ucb::configureUcb(): Bad argument placeholders");
    }

    return true;
}

}

//============================================================================
//
//  unconfigureUcb
//
//============================================================================

namespace ucb {

void
unconfigureUcb(
    uno::Reference< star::ucb::XContentProviderManager > const & rManager,
    std::vector< ContentProviderRegistrationInfo > const & rInfos)
    throw (uno::RuntimeException)
{
    std::vector< ContentProviderRegistrationInfo >::const_iterator
        aEnd(rInfos.end());
    for (std::vector< ContentProviderRegistrationInfo >::const_iterator
             aIt(rInfos.begin());
         aIt != aEnd; ++aIt)
        deregisterFromUcb(rManager, *aIt);
}

}
