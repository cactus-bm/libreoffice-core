/*************************************************************************
 *
 *  $RCSfile: VKey.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: oj $ $Date: 2001-05-23 09:10:28 $
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


#ifndef _CONNECTIVITY_SDBCX_KEY_HXX_
#include "connectivity/sdbcx/VKey.hxx"
#endif
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_KEYRULE_HPP_
#include <com/sun/star/sdbc/KeyRule.hpp>
#endif
#ifndef _COMPHELPER_SEQUENCE_HXX_
#include <comphelper/sequence.hxx>
#endif
#ifndef _CONNECTIVITY_SDBCX_COLUMN_HXX_
#include "connectivity/sdbcx/VColumn.hxx"
#endif
#ifndef _CONNECTIVITY_SDBCX_COLLECTION_HXX_
#include "connectivity/sdbcx/VCollection.hxx"
#endif
#ifndef CONNECTIVITY_CONNECTION_HXX
#include "TConnection.hxx"
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

// -----------------------------------------------------------------------------
::rtl::OUString SAL_CALL OKey::getImplementationName(  ) throw (::com::sun::star::uno::RuntimeException)
{
    if(isNew())
        return ::rtl::OUString::createFromAscii("com.sun.star.sdbcx.VKeyDescription");
    return ::rtl::OUString::createFromAscii("com.sun.star.sdbcx.VKey");
}
// -----------------------------------------------------------------------------
::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL OKey::getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException)
{
    ::com::sun::star::uno::Sequence< ::rtl::OUString > aSupported(1);
    if(isNew())
        aSupported[0] = ::rtl::OUString::createFromAscii("com.sun.star.sdbcx.KeyDescription");
    else
        aSupported[0] = ::rtl::OUString::createFromAscii("com.sun.star.sdbcx.Key");

    return aSupported;
}
// -----------------------------------------------------------------------------
sal_Bool SAL_CALL OKey::supportsService( const ::rtl::OUString& _rServiceName ) throw(::com::sun::star::uno::RuntimeException)
{
    Sequence< ::rtl::OUString > aSupported(getSupportedServiceNames());
    const ::rtl::OUString* pSupported = aSupported.getConstArray();
    const ::rtl::OUString* pEnd = pSupported + aSupported.getLength();
    for (;pSupported != pEnd && !pSupported->equals(_rServiceName); ++pSupported)
        ;

    return pSupported != pEnd;
}
// -------------------------------------------------------------------------
OKey::OKey(sal_Bool _bCase) :   ODescriptor_BASE(m_aMutex)
            ,   ODescriptor(ODescriptor_BASE::rBHelper,_bCase,sal_True)
            ,   m_pColumns(NULL)
{
}
// -------------------------------------------------------------------------
OKey::OKey( const ::rtl::OUString& _Name,
            const ::rtl::OUString& _ReferencedTable,
            sal_Int32       _Type,
            sal_Int32       _UpdateRule,
            sal_Int32       _DeleteRule,
            sal_Bool _bCase) :  ODescriptor_BASE(m_aMutex)
                        ,ODescriptor(ODescriptor_BASE::rBHelper,_bCase)
                        ,m_pColumns(NULL)
                        ,m_ReferencedTable(_ReferencedTable)
                        ,m_Type(_Type)
                        ,m_UpdateRule(_UpdateRule)
                        ,m_DeleteRule(_DeleteRule)
{
    m_Name = _Name;
}
// -------------------------------------------------------------------------
OKey::~OKey( )
{
    delete m_pColumns;
}
// -------------------------------------------------------------------------
Any SAL_CALL OKey::queryInterface( const Type & rType ) throw(RuntimeException)
{
    Any aRet = ODescriptor::queryInterface( rType);
    if(!aRet.hasValue())
    {
        if(!isNew())
            aRet = OKey_BASE::queryInterface(rType);
        if(!aRet.hasValue())
            aRet = ODescriptor_BASE::queryInterface( rType);
    }

    return aRet;
}
// -------------------------------------------------------------------------
Sequence< Type > SAL_CALL OKey::getTypes(  ) throw(RuntimeException)
{
    if(isNew())
        return ::comphelper::concatSequences(ODescriptor::getTypes(),ODescriptor_BASE::getTypes());

    return ::comphelper::concatSequences(ODescriptor::getTypes(),ODescriptor_BASE::getTypes(),OKey_BASE::getTypes());
}
// -------------------------------------------------------------------------
void OKey::construct()
{
    ODescriptor::construct();

    sal_Int32 nAttrib = isNew() ? 0 : PropertyAttribute::READONLY;

    registerProperty(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_REFERENCEDTABLE), PROPERTY_ID_REFERENCEDTABLE,    nAttrib,&m_ReferencedTable, ::getCppuType(reinterpret_cast< ::rtl::OUString*>(NULL)));
    registerProperty(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE),                PROPERTY_ID_TYPE,               nAttrib,&m_Type,            ::getCppuType(reinterpret_cast<sal_Int32*>(NULL)));
    registerProperty(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_UPDATERULE),      PROPERTY_ID_UPDATERULE,         nAttrib,&m_UpdateRule,      ::getCppuType(reinterpret_cast<sal_Int32*>(NULL)));
    registerProperty(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_DELETERULE),      PROPERTY_ID_DELETERULE,         nAttrib,&m_DeleteRule,      ::getCppuType(reinterpret_cast<sal_Int32*>(NULL)));
}
// -------------------------------------------------------------------------
void SAL_CALL OKey::disposing()
{
    OPropertySetHelper::disposing();

    ::osl::MutexGuard aGuard(m_aMutex);

    if(m_pColumns)
        m_pColumns->disposing();

    ODescriptor_BASE::disposing();
}
// -------------------------------------------------------------------------
::cppu::IPropertyArrayHelper* OKey::createArrayHelper(sal_Int32 _nId ) const
{
    Sequence< Property > aProps;
    describeProperties(aProps);
    changePropertyAttributte(aProps);
    return new ::cppu::OPropertyArrayHelper(aProps);
}
// -------------------------------------------------------------------------
::cppu::IPropertyArrayHelper & OKey::getInfoHelper()
{
    return *const_cast<OKey*>(this)->getArrayHelper(isNew() ? 1 : 0);
}
// -------------------------------------------------------------------------
Reference< ::com::sun::star::container::XNameAccess > SAL_CALL OKey::getColumns(  ) throw(RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    checkDisposed(ODescriptor_BASE::rBHelper.bDisposed);


    if(!m_pColumns)
        refreshColumns();

    return const_cast<OKey*>(this)->m_pColumns;
}
// -------------------------------------------------------------------------
Reference< XPropertySet > SAL_CALL OKey::createDataDescriptor(  ) throw(RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    checkDisposed(ODescriptor_BASE::rBHelper.bDisposed);


    return this;
}
// -----------------------------------------------------------------------------
::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL OKey::getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException)
{
    return ::cppu::OPropertySetHelper::createPropertySetInfo(getInfoHelper());
}
// -----------------------------------------------------------------------------
::rtl::OUString SAL_CALL OKey::getName(  ) throw(::com::sun::star::uno::RuntimeException)
{
    return m_Name;
}
// -----------------------------------------------------------------------------
void SAL_CALL OKey::setName( const ::rtl::OUString& aName ) throw(::com::sun::star::uno::RuntimeException)
{
}
// -----------------------------------------------------------------------------
// XInterface
void SAL_CALL OKey::acquire() throw(::com::sun::star::uno::RuntimeException)
{
    ODescriptor_BASE::acquire();
}
// -----------------------------------------------------------------------------
void SAL_CALL OKey::release() throw(::com::sun::star::uno::RuntimeException)
{
    ODescriptor_BASE::release();
}
// -----------------------------------------------------------------------------

