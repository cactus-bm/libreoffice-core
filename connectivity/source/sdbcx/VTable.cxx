/*************************************************************************
 *
 *  $RCSfile: VTable.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: oj $ $Date: 2000-10-09 12:06:36 $
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

#ifndef _CONNECTIVITY_SDBCX_TABLE_HXX_
#include "connectivity/sdbcx/VTable.hxx"
#endif

#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#ifndef _CONNECTIVITY_PROPERTYIDS_HXX_
#include "propertyids.hxx"
#endif
#ifndef _CONNECTIVITY_SDBCX_COLUMN_HXX_
#include "connectivity/sdbcx/VColumn.hxx"
#endif
#ifndef _CONNECTIVITY_SDBCX_KEY_HXX_
#include "connectivity/sdbcx/VKey.hxx"
#endif
#ifndef _CONNECTIVITY_SDBCX_INDEX_HXX_
#include "connectivity/sdbcx/VIndex.hxx"
#endif
#ifndef _COMPHELPER_SEQUENCE_HXX_
#include <comphelper/sequence.hxx>
#endif

// -------------------------------------------------------------------------
using namespace connectivity;
using namespace connectivity::sdbcx;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;

IMPLEMENT_SERVICE_INFO(OTable,"com.sun.star.sdbcx.VTable","com.sun.star.sdbcx.Table");
// -------------------------------------------------------------------------
OTable::OTable(sal_Bool _bCase) :   OTable_BASE(m_aMutex)
                ,ODescriptor(OTable_BASE::rBHelper,_bCase,sal_True)
                ,m_pIndexes(NULL)
                ,m_pKeys(NULL)
                ,m_pColumns(NULL)
{
}
// --------------------------------------------------------------------------
OTable::OTable( sal_Bool _bCase,
                const ::rtl::OUString& _Name,       const ::rtl::OUString& _Type,
                const ::rtl::OUString& _Description,const ::rtl::OUString& _SchemaName,
                const ::rtl::OUString& _CatalogName) :  OTable_BASE(m_aMutex)
                ,ODescriptor(OTable_BASE::rBHelper,_bCase)
                ,m_pIndexes(NULL)
                ,m_pKeys(NULL)
                ,m_pColumns(NULL)
                ,m_CatalogName(_CatalogName)
                ,m_SchemaName(_SchemaName)
                ,m_Description(_Description)
                ,m_Type(_Type)
{
    m_Name = _Name;
}
// -------------------------------------------------------------------------
Any SAL_CALL OTable::queryInterface( const Type & rType ) throw(RuntimeException)
{
        Any aRet = ODescriptor::queryInterface( rType);
    if(aRet.hasValue())
        return aRet;
    return OTable_BASE::queryInterface( rType);
}
// -------------------------------------------------------------------------
Sequence< Type > SAL_CALL OTable::getTypes(  ) throw(RuntimeException)
{
    return ::comphelper::concatSequences(ODescriptor::getTypes(),OTable_BASE::getTypes());
}
// -------------------------------------------------------------------------
void OTable::construct()
{
    ODescriptor::construct();

        sal_Int32 nAttrib = isNew() ? 0 : PropertyAttribute::READONLY;

    registerProperty(PROPERTY_CATALOGNAME,      PROPERTY_ID_CATALOGNAME,nAttrib,&m_CatalogName, ::getCppuType(reinterpret_cast< ::rtl::OUString*>(NULL)));
    registerProperty(PROPERTY_SCHEMANAME,       PROPERTY_ID_SCHEMANAME, nAttrib,&m_SchemaName,  ::getCppuType(reinterpret_cast< ::rtl::OUString*>(NULL)));
    registerProperty(PROPERTY_DESCRIPTION,      PROPERTY_ID_DESCRIPTION,nAttrib,&m_Description, ::getCppuType(reinterpret_cast< ::rtl::OUString*>(NULL)));
    registerProperty(PROPERTY_TYPE,             PROPERTY_ID_TYPE,       nAttrib,&m_Type,        ::getCppuType(reinterpret_cast< ::rtl::OUString*>(NULL)));
}
// -------------------------------------------------------------------------
void SAL_CALL OTable::disposing(void)
{
    ODescriptor::disposing();

    ::osl::MutexGuard aGuard(m_aMutex);

    if(m_pIndexes)
        m_pIndexes->disposing();
    if(m_pKeys)
        m_pKeys->disposing();
    if(m_pColumns)
        m_pColumns->disposing();

}
// -------------------------------------------------------------------------
::cppu::IPropertyArrayHelper* OTable::createArrayHelper( ) const
{
        Sequence< Property > aProps;
    describeProperties(aProps);
    return new ::cppu::OPropertyArrayHelper(aProps);
}
// -------------------------------------------------------------------------
::cppu::IPropertyArrayHelper & OTable::getInfoHelper()
{
    return *const_cast<OTable*>(this)->getArrayHelper();
}
// -------------------------------------------------------------------------
Reference< XPropertySet > SAL_CALL OTable::createDataDescriptor(  ) throw(RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    if (OTable_BASE::rBHelper.bDisposed)
                throw DisposedException();

    return this;
}
// XColumnsSupplier
Reference< XNameAccess > SAL_CALL OTable::getColumns(  ) throw(RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    if (OTable_BASE::rBHelper.bDisposed)
                throw DisposedException();

    if(!m_pColumns)
        refreshColumns();

    return const_cast<OTable*>(this)->m_pColumns;
}
// -------------------------------------------------------------------------
// XIndexesSupplier
Reference< XNameAccess > SAL_CALL OTable::getIndexes(  ) throw(RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    if (OTable_BASE::rBHelper.bDisposed)
                throw DisposedException();

    if(!m_pIndexes)
        refreshIndexes();

    return const_cast<OTable*>(this)->m_pIndexes;
}
// -------------------------------------------------------------------------
// XKeysSupplier
Reference< XIndexAccess > SAL_CALL OTable::getKeys(  ) throw(RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    if (OTable_BASE::rBHelper.bDisposed)
                throw DisposedException();

    if(!m_pKeys)
        refreshKeys();

    return const_cast<OTable*>(this)->m_pKeys;
}
// -------------------------------------------------------------------------
// XRename
void SAL_CALL OTable::rename( const ::rtl::OUString& newName ) throw(SQLException, ElementExistException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    if (OTable_BASE::rBHelper.bDisposed)
                throw DisposedException();

}
// -------------------------------------------------------------------------
// XAlterTable
void SAL_CALL OTable::alterColumnByName( const ::rtl::OUString& colName, const Reference< XPropertySet >& descriptor ) throw(SQLException, NoSuchElementException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    if (OTable_BASE::rBHelper.bDisposed)
                throw DisposedException();

}
// -------------------------------------------------------------------------
void SAL_CALL OTable::alterColumnByIndex( sal_Int32 index, const Reference< XPropertySet >& descriptor ) throw(SQLException, ::com::sun::star::lang::IndexOutOfBoundsException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    if (OTable_BASE::rBHelper.bDisposed)
                throw DisposedException();

}
// -------------------------------------------------------------------------


