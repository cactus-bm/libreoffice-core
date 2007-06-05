/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: providermap.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: ihi $ $Date: 2007-06-05 17:52:07 $
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

#ifndef _UCB_PROVIDERMAP_HXX_
#define _UCB_PROVIDERMAP_HXX_

#include <list>

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _UCB_REGEXPMAP_HXX_
#include <regexpmap.hxx>
#endif

namespace com { namespace sun { namespace star { namespace ucb {
    class XContentProvider;
} } } }

//============================================================================
class ProviderListEntry_Impl
{
    com::sun::star::uno::Reference<
        com::sun::star::ucb::XContentProvider > m_xProvider;
    mutable com::sun::star::uno::Reference<
        com::sun::star::ucb::XContentProvider > m_xResolvedProvider;

private:
    com::sun::star::uno::Reference<
        com::sun::star::ucb::XContentProvider > resolveProvider() const;

public:
    ProviderListEntry_Impl(
        const com::sun::star::uno::Reference<
            com::sun::star::ucb::XContentProvider >& xProvider )
    : m_xProvider( xProvider ) {}

    com::sun::star::uno::Reference<
        com::sun::star::ucb::XContentProvider > getProvider() const
    { return m_xProvider; }
    inline com::sun::star::uno::Reference<
        com::sun::star::ucb::XContentProvider > getResolvedProvider() const;
};

inline com::sun::star::uno::Reference< com::sun::star::ucb::XContentProvider >
ProviderListEntry_Impl::getResolvedProvider() const
{
    return m_xResolvedProvider.is() ? m_xResolvedProvider : resolveProvider();
}

//============================================================================
typedef std::list< ProviderListEntry_Impl > ProviderList_Impl;

//============================================================================
typedef ucb_impl::RegexpMap< ProviderList_Impl > ProviderMap_Impl;

#endif // _UCB_PROVIDERMAP_HXX_

