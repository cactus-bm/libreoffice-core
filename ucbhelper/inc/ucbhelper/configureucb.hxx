/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: configureucb.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: ihi $ $Date: 2007-06-05 14:48:12 $
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
#define _UCBHELPER_CONFIGUREUCB_HXX_

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HPP_
#include <com/sun/star/uno/RuntimeException.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#include <vector>
#ifndef INCLUDED_UCBHELPERDLLAPI_H
#include "ucbhelper/ucbhelperdllapi.h"
#endif

namespace com { namespace sun { namespace star {
    namespace lang { class XMultiServiceFactory; }
    namespace ucb {
        class XContentProvider;
        class XContentProviderManager;
    }
    namespace uno { class Any; }
} } }

namespace ucbhelper {

//============================================================================
/** Information about a registered content provider, passed from
    <method>configureUcb</method> to <method>unconfigureUcb</method> (and from
    <method>registerAtUcb</method> to <method>deregisterFromUcb</method>).
 */
struct ContentProviderRegistrationInfo
{
    /** The registered content provider (or null if registration failed).
     */
    com::sun::star::uno::Reference< com::sun::star::ucb::XContentProvider >
        m_xProvider;

    /** The arguments the content provider was instantiated with.
     */
    rtl::OUString m_aArguments;

    /** The URL template the content provider is registered on.
     */
    rtl::OUString m_aTemplate;
};
    //@@@ if registerucb.hxx were exported, too, this might better reside in
    // there...

typedef std::vector< ContentProviderRegistrationInfo >
                                    ContentProviderRegistrationInfoList;

//============================================================================
/** Information about a content provider, passed to
    <method>configureUcb</method>.
 */
struct ContentProviderData
{
    /** The UNO service name to use to instanciate the content provider.
     */
    rtl::OUString ServiceName;

    /** The URL template to use to instanciate the content provider.
     */
    rtl::OUString URLTemplate;

    /** The arguments to use to instanciate the content provider.
     */
    rtl::OUString Arguments;

    ContentProviderData() {};
    ContentProviderData( const rtl::OUString & rService,
                         const rtl::OUString & rTemplate,
                         const rtl::OUString & rArgs )
    : ServiceName( rService ), URLTemplate( rTemplate ), Arguments( rArgs ) {}
};

typedef std::vector< ContentProviderData > ContentProviderDataList;

//============================================================================
/** Configure a (newly instantiated) Universal Content Broker.

    @descr  This function tries to register at the given content provider
    manager all the content provider services listed under a given key in the
    configuration database.

    @param rManager  A content provider manager (normally, this would be a
    newly intantiated UCB).

    @param rServiceFactory  A service factory through which to obtain the
    various services required.

    @param rData  A list containing the data for the content providers for
    the UCB to configure.

    @pInfos  If not null, an entry will be added to this vector for every
    content provider that is registered (sucessfully or not).

    @return  True if the UCB has successfuly been configured (though not all
    content providers have necessarily been registered due to individual
    problems).
 */
bool
configureUcb(
    com::sun::star::uno::Reference<
            com::sun::star::ucb::XContentProviderManager > const &
        rManager,
    com::sun::star::uno::Reference<
            com::sun::star::lang::XMultiServiceFactory > const &
        rServiceFactory,
    ContentProviderDataList const & rData,
    ContentProviderRegistrationInfoList * pInfos)
    throw (com::sun::star::uno::RuntimeException);


//============================================================================
/** Configure a (newly instantiated) Universal Content Broker.

    @descr  This function tries to register at the given content provider
    manager all the content provider services listed under a given key in the
    configuration database.

    @param rManager  A content provider manager (normally, this would be a
    newly intantiated UCB).

    @param rServiceFactory  A service factory through which to obtain the
    various services required.

    @param rArguments  A sequence of at least two strings: the primary and
    secondary key addressing a chosen UCB configuration in the configuration
    database.  The sequence can be longer, in which case the excess elements
    must be strings that form key/value pairs.  These key/value pairs will be
    used to replace placeholders in the data from the configuration database
    with (dynamic) values.  This is a (rather unstructured) sequence of
    <type>Any<type/>s, since normally this parameter will simply be forwarded
    by the UCB's <method>initialize<method/> method, which has a parameter of
    the same type.

    @pInfos  If not null, an entry will be added to this vector for every
    content provider that is registered (sucessfully or not).

    @return  True if the UCB has successfuly been configured (though not all
    content providers have necessarily been registered due to individual
    problems).
 */
UCBHELPER_DLLPUBLIC bool
configureUcb(
    com::sun::star::uno::Reference<
            com::sun::star::ucb::XContentProviderManager > const &
        rManager,
    com::sun::star::uno::Reference<
            com::sun::star::lang::XMultiServiceFactory > const &
        rServiceFactory,
    com::sun::star::uno::Sequence< com::sun::star::uno::Any > const &
        rArguments,
    std::vector< ContentProviderRegistrationInfo > * pInfos)
    throw (com::sun::star::uno::RuntimeException);

//============================================================================
/** Undo the configuration of a Universal Content Broker.

    @descr  This function is the reverse of <method>configureUcb</method>.

    @param rManager  A content provider manager.

    @param rInfos  Information about all the registered content providers.
 */
void
unconfigureUcb(
    com::sun::star::uno::Reference<
            com::sun::star::ucb::XContentProviderManager > const &
        rManager,
    std::vector< ContentProviderRegistrationInfo > const & rInfos)
    throw (com::sun::star::uno::RuntimeException);

}

#endif // _UCBHELPER_CONFIGUREUCB_HXX_
