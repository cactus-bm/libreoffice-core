/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: webdavdatasupplier.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2006-01-19 18:31:51 $
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

#ifndef _WEBDAV_UCP_DATASUPPLIER_HXX
#define _WEBDAV_UCP_DATASUPPLIER_HXX

#include <vector>

#ifndef _RTL_REF_HXX_
#include <rtl/ref.hxx>
#endif

#ifndef _UCBHELPER_RESULTSET_HXX
#include <ucbhelper/resultset.hxx>
#endif

namespace webdav_ucp {

struct DataSupplier_Impl;
class Content;
struct DAVResource;
class ContentProperties;

class DataSupplier : public ucb::ResultSetDataSupplier
{
    DataSupplier_Impl* m_pImpl;

private:
    sal_Bool getData();

public:
    DataSupplier( const com::sun::star::uno::Reference<
              com::sun::star::lang::XMultiServiceFactory >& rxSMgr,
              const rtl::Reference< Content >& rContent,
              sal_Int32 nOpenMode);

    virtual ~DataSupplier();

    virtual rtl::OUString queryContentIdentifierString( sal_uInt32 nIndex );
    virtual com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifier >
    queryContentIdentifier( sal_uInt32 nIndex );
    virtual com::sun::star::uno::Reference< com::sun::star::ucb::XContent >
    queryContent( sal_uInt32 nIndex );

    virtual sal_Bool getResult( sal_uInt32 nIndex );

    virtual sal_uInt32 totalCount();
    virtual sal_uInt32 currentCount();
    virtual sal_Bool isCountFinal();

    virtual com::sun::star::uno::Reference< com::sun::star::sdbc::XRow >
    queryPropertyValues( sal_uInt32 nIndex  );
    virtual void releasePropertyValues( sal_uInt32 nIndex );

    virtual void close();

    virtual void validate()
        throw( com::sun::star::ucb::ResultSetException );
};

}

#endif
