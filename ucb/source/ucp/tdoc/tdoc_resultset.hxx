/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: tdoc_resultset.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: ihi $ $Date: 2007-06-05 18:18:08 $
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

#ifndef INCLUDED_TDOC_RESULTSET_HXX
#define INCLUDED_TDOC_RESULTSET_HXX

#ifndef _RTL_REF_HXX_
#include <rtl/ref.hxx>
#endif
#ifndef _UCBHELPER_RESULTSETHELPER_HXX
#include <ucbhelper/resultsethelper.hxx>
#endif

namespace tdoc_ucp {

class Content;

class DynamicResultSet : public ::ucbhelper::ResultSetImplHelper
{
    rtl::Reference< Content > m_xContent;

private:
    virtual void initStatic();
    virtual void initDynamic();

public:
    DynamicResultSet(
            const com::sun::star::uno::Reference<
                com::sun::star::lang::XMultiServiceFactory >& rxSMgr,
            const rtl::Reference< Content >& rxContent,
            const com::sun::star::ucb::OpenCommandArgument2& rCommand );
};

}

#endif /* !INCLUDED_TDOC_RESULTSET_HXX */
