/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dp_misc.h,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 17:18:39 $
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

#define OUSTR(x) ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(x) )
#define ARLEN(x) (sizeof (x) / sizeof *(x))


namespace css = ::com::sun::star;

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
inline void try_dispose( css::uno::Reference<css::uno::XInterface> const & x )
{
    css::uno::Reference<css::lang::XComponent> xComp( x, css::uno::UNO_QUERY );
    if (xComp.is())
        xComp->dispose();
}

//##############################################################################

//==============================================================================
::rtl::OUString expandUnoRcTerm( ::rtl::OUString const & term );

//==============================================================================
::rtl::OUString expandUnoRcUrl( ::rtl::OUString const & url );

//==============================================================================
::rtl::OUString const & getPlatformString();

//==============================================================================
bool platform_fits( ::rtl::OUString const & platform_string );

//==============================================================================
::rtl::OUString makeURL(
    ::rtl::OUString const & baseURL, ::rtl::OUString const & relPath );

//==============================================================================
::rtl::OUString generateRandomPipeId();

class AbortChannel;
//==============================================================================
css::uno::Reference<css::uno::XInterface> resolveUnoURL(
    ::rtl::OUString const & connectString,
    css::uno::Reference<css::uno::XComponentContext> const & xLocalContext,
    AbortChannel * abortChannel = 0 );

//==============================================================================
bool office_is_running();

//==============================================================================
oslProcess raiseProcess( ::rtl::OUString const & appURL,
                         css::uno::Sequence< ::rtl::OUString > const & args );

}

#endif
