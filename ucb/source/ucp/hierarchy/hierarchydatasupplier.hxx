/*************************************************************************
 *
 *  $RCSfile: hierarchydatasupplier.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kso $ $Date: 2001-06-25 09:08:40 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _HIERARCHYDATASUPPLIER_HXX
#define _HIERARCHYDATASUPPLIER_HXX

#ifndef _RTL_REF_HXX_
#include <rtl/ref.hxx>
#endif

#ifndef _COM_SUN_STAR_UCB_OPENMODE_HPP_
#include <com/sun/star/ucb/OpenMode.hpp>
#endif

#ifndef _UCBHELPER_RESULTSET_HXX
#include <ucbhelper/resultset.hxx>
#endif

namespace hierarchy_ucp {

struct HierarchyEntryData;
struct DataSupplier_Impl;
class HierarchyContent;

class HierarchyResultSetDataSupplier : public ucb::ResultSetDataSupplier
{
    DataSupplier_Impl*  m_pImpl;

private:
    sal_Bool checkResult( const HierarchyEntryData& rResult );

public:
    HierarchyResultSetDataSupplier(
                    const com::sun::star::uno::Reference<
                        com::sun::star::lang::XMultiServiceFactory >& rxSMgr,
                    const rtl::Reference< HierarchyContent >& rContent,
                    sal_Int32 nOpenMode = com::sun::star::ucb::OpenMode::ALL );
    virtual ~HierarchyResultSetDataSupplier();

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

} // namespace hierarchy_ucp

#endif /* !_HIERARCHYDATASUPPLIER_HXX */
