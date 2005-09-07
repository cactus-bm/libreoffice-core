/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: OIndexContainer.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 00:44:16 $
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
#ifndef CHART_OINDEXCONTAINER_HXX
#define CHART_OINDEXCONTAINER_HXX

#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XINDEXCONTAINER_HPP_
#include <com/sun/star/container/XIndexContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XTYPEPROVIDER_HPP_
#include <com/sun/star/lang/XTypeProvider.hpp>
#endif

#include <vector>

namespace comphelper
{

class OIndexContainer :
        public ::com::sun::star::container::XIndexContainer
{
public:
    explicit OIndexContainer( ::osl::Mutex & rMutex );
    virtual ~OIndexContainer();

    ::osl::Mutex & GetMutex();

    // ____ XInterface ____
    virtual ::com::sun::star::uno::Any SAL_CALL
        queryInterface( const ::com::sun::star::uno::Type& aType )
        throw (::com::sun::star::uno::RuntimeException);

protected:
    /// implemented as a standard-library vector
    typedef ::std::vector< ::com::sun::star::uno::Any > tHelperContainerType;

    /// allows direct standard-library access to the container
    tHelperContainerType      m_aContainer;

    // you have to implement this in derived classes
    // ____ XElementAccess ____
//     virtual ::com::sun::star::uno::Type SAL_CALL getElementType()
//         throw (::com::sun::star::uno::RuntimeException) = 0;

    // implemented by this class
    // =========================

    // ____ XIndexContainer ____
    virtual void SAL_CALL insertByIndex( sal_Int32 Index, const ::com::sun::star::uno::Any& Element )
        throw (::com::sun::star::lang::IllegalArgumentException,
               ::com::sun::star::lang::IndexOutOfBoundsException,
               ::com::sun::star::lang::WrappedTargetException,
               ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeByIndex( sal_Int32 Index )
        throw (::com::sun::star::lang::IndexOutOfBoundsException,
               ::com::sun::star::lang::WrappedTargetException,
               ::com::sun::star::uno::RuntimeException);

    // ____ XIndexReplace ____
    virtual void SAL_CALL replaceByIndex( sal_Int32 Index, const ::com::sun::star::uno::Any& Element )
        throw (::com::sun::star::lang::IllegalArgumentException,
               ::com::sun::star::lang::IndexOutOfBoundsException,
               ::com::sun::star::lang::WrappedTargetException,
               ::com::sun::star::uno::RuntimeException);

    // ____ XIndexAccess ____
    virtual sal_Int32 SAL_CALL getCount()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index )
        throw (::com::sun::star::lang::IndexOutOfBoundsException,
               ::com::sun::star::lang::WrappedTargetException,
               ::com::sun::star::uno::RuntimeException);

    // ____ XElementAccess ____
    virtual sal_Bool SAL_CALL hasElements()
        throw (::com::sun::star::uno::RuntimeException);

private:
    ::osl::Mutex &  m_rMutex;
};

} //  namespace comphelper

// CHART_OINDEXCONTAINER_HXX
#endif
