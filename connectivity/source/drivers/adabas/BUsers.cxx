/*************************************************************************
 *
 *  $RCSfile: BUsers.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: oj $ $Date: 2001-06-20 12:22:56 $
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

#ifndef _CONNECTIVITY_ADABAS_USERS_HXX_
#include "adabas/BUsers.hxx"
#endif
#ifndef _CONNECTIVITY_ADABAS_USER_HXX_
#include "adabas/BUser.hxx"
#endif
#ifndef _CONNECTIVITY_ADABAS_TABLE_HXX_
#include "adabas/BTable.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSET_HPP_
#include <com/sun/star/sdbc/XResultSet.hpp>
#endif
#ifndef _CONNECTIVITY_SDBCX_IREFRESHABLE_HXX_
#include "connectivity/sdbcx/IRefreshable.hxx"
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif

using namespace ::comphelper;

using namespace connectivity::adabas;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
//  using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
typedef connectivity::sdbcx::OCollection OCollection_TYPE;

Reference< XNamed > OUsers::createObject(const ::rtl::OUString& _rName)
{
    OAdabasUser* pRet = new OAdabasUser(m_pConnection,_rName);
        Reference< XNamed > xRet = pRet;
    return xRet;
}
// -------------------------------------------------------------------------
void OUsers::impl_refresh() throw(RuntimeException)
{
    m_pParent->refreshUsers();
}
// -------------------------------------------------------------------------
Reference< XPropertySet > OUsers::createEmptyObject()
{
    OUserExtend* pNew = new OUserExtend(m_pConnection);
    return pNew;
}
// -------------------------------------------------------------------------
// XAppend
void SAL_CALL OUsers::appendByDescriptor( const Reference< XPropertySet >& descriptor ) throw(SQLException, ElementExistException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_rMutex);
    ::rtl::OUString aName = getString(descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)));
    ObjectMap::iterator aIter = m_aNameMap.find(aName);
    if( aIter != m_aNameMap.end())
        throw ElementExistException(aName,*this);

    ::rtl::OUString aSql    = ::rtl::OUString::createFromAscii("CREATE USER ");
    ::rtl::OUString aQuote  = m_pConnection->getMetaData()->getIdentifierQuoteString(  );

    ::rtl::OUString sUserName;
    descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= sUserName;
    sUserName = sUserName.toAsciiUpperCase();
    descriptor->setPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME),makeAny(sUserName));
    aSql = aSql + aQuote + sUserName + aQuote
                + ::rtl::OUString::createFromAscii(" PASSWORD ")
                + getString(descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_PASSWORD)));
    aSql += ::rtl::OUString::createFromAscii(" RESOURCE NOT EXCLUSIVE");

    Reference< XStatement > xStmt = m_pConnection->createStatement(  );
    xStmt->execute(aSql);

    OCollection_TYPE::appendByDescriptor(descriptor);
}
// -------------------------------------------------------------------------
// XDrop
void SAL_CALL OUsers::dropByName( const ::rtl::OUString& elementName ) throw(SQLException, NoSuchElementException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_rMutex);
    ObjectMap::iterator aIter = m_aNameMap.find(elementName);
    if( aIter == m_aNameMap.end())
        throw NoSuchElementException(elementName,*this);

    ::rtl::OUString aSql    = ::rtl::OUString::createFromAscii("DROP USER ");
    ::rtl::OUString aQuote  = m_pConnection->getMetaData()->getIdentifierQuoteString(  );

    aSql = aSql + aQuote + elementName + aQuote;

        Reference< XStatement > xStmt = m_pConnection->createStatement(  );
    xStmt->execute(aSql);

    OCollection_TYPE::dropByName(elementName);
}

// -------------------------------------------------------------------------
void SAL_CALL OUsers::dropByIndex( sal_Int32 index ) throw(SQLException, IndexOutOfBoundsException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_rMutex);
    if (index < 0 || index >= getCount())
        throw IndexOutOfBoundsException(::rtl::OUString::valueOf(index),*this);

    dropByName(m_aElements[index]->first);
}
// -----------------------------------------------------------------------------


