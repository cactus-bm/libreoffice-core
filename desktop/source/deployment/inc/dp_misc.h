/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: dp_misc.h,v $
 * $Revision: 1.13 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#if ! defined INCLUDED_DP_MISC_H
#define INCLUDED_DP_MISC_H

#include "rtl/ustrbuf.hxx"
#include "rtl/instance.hxx"
#include "osl/mutex.hxx"
#include "osl/process.h"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/lang/XComponent.hpp"
#include "com/sun/star/lang/DisposedException.hpp"
#include "com/sun/star/deployment/XPackageRegistry.hpp"
#include "com/sun/star/awt/XWindow.hpp"
#include "dp_misc_api.hxx"

#define OUSTR(x) ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(x) )
#define ARLEN(x) (sizeof (x) / sizeof *(x))

namespace dp_misc {

const sal_Char CR = 0x0d;
const sal_Char LF = 0x0a;

//==============================================================================
class MutexHolder
{
    mutable ::osl::Mutex m_mutex;
protected:
    inline ::osl::Mutex & getMutex() const { return m_mutex; }
};

//==============================================================================
inline void try_dispose( ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> const & x )
{
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent> xComp( x, ::com::sun::star::uno::UNO_QUERY );
    if (xComp.is())
        xComp->dispose();
}

//##############################################################################

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::rtl::OUString expandUnoRcTerm( ::rtl::OUString const & term );

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::rtl::OUString expandUnoRcUrl( ::rtl::OUString const & url );

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC ::rtl::OUString makeURL(
    ::rtl::OUString const & baseURL, ::rtl::OUString const & relPath );

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC ::rtl::OUString generateRandomPipeId();

class AbortChannel;
//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> resolveUnoURL(
    ::rtl::OUString const & connectString,
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext> const & xLocalContext,
    AbortChannel * abortChannel = 0 );

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC bool office_is_running();

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
oslProcess raiseProcess( ::rtl::OUString const & appURL,
                         ::com::sun::star::uno::Sequence< ::rtl::OUString > const & args );

//==============================================================================
/** returns the default update URL (for the update information) which
    is used when an extension does not provide its own URL.
*/
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::rtl::OUString getExtensionDefaultUpdateURL();


}

#endif
