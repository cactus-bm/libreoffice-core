/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
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
 *  The Initial Developer of the Original Code is: Joerg Budischewski
 *
 *   Copyright: 2000 by Sun Microsystems, Inc.
 *              2011 by Lionel Elie Mamane <lionel@mamane.lu>
 *
 *   All Rights Reserved.
 *
 *   Contributor(s): Joerg Budischewski
 *
 ************************************************************************/

#ifndef PQ_XVIEW_HXX
#define PQ_XVIEW_HXX

#include <cppuhelper/component.hxx>
#include <cppuhelper/propshlp.hxx>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/sdbcx/XDataDescriptorFactory.hpp>
#include <com/sun/star/sdbcx/XRename.hpp>
#include <com/sun/star/container/XNamed.hpp>

#include "pq_xbase.hxx"

namespace pq_sdbc_driver
{
class Views;
class View : public ReflectionBase,
             public com::sun::star::sdbcx::XRename
{
public:
    View( const ::rtl::Reference< RefCountedMutex > & refMutex,
          const ::com::sun::star::uno::Reference< com::sun::star::sdbc::XConnection > & connection,
          ConnectionSettings *pSettings);
public:
    void refetch();

public: // XInterface
    virtual void SAL_CALL acquire() throw() { OComponentHelper::acquire(); }
    virtual void SAL_CALL release() throw() { OComponentHelper::release(); }
    virtual com::sun::star::uno::Any  SAL_CALL queryInterface(
        const com::sun::star::uno::Type & reqType )
        throw (com::sun::star::uno::RuntimeException);

public: // XTypeProvider, first implemented by OPropertySetHelper
    virtual com::sun::star::uno::Sequence< com::sun::star::uno::Type > SAL_CALL getTypes()
        throw( com::sun::star::uno::RuntimeException );
    virtual com::sun::star::uno::Sequence< sal_Int8> SAL_CALL getImplementationId()
        throw( com::sun::star::uno::RuntimeException );

public: // XDataDescriptorFactory
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL
    createDataDescriptor(  ) throw (::com::sun::star::uno::RuntimeException);

public: // XRename
    virtual void SAL_CALL rename( const ::rtl::OUString& newName )
        throw (::com::sun::star::sdbc::SQLException,
               ::com::sun::star::container::ElementExistException,
               ::com::sun::star::uno::RuntimeException);

public: // XNamed
    virtual ::rtl::OUString SAL_CALL getName(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setName( const ::rtl::OUString& aName ) throw (::com::sun::star::uno::RuntimeException);

};


class ViewDescriptor : public ReflectionBase
{
public:
    ViewDescriptor( const ::rtl::Reference< RefCountedMutex > & refMutex,
          const ::com::sun::star::uno::Reference< com::sun::star::sdbc::XConnection > & connection,
          ConnectionSettings *pSettings);

public: // XDataDescriptorFactory
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL
    createDataDescriptor(  ) throw (::com::sun::star::uno::RuntimeException);
};

}


#endif
