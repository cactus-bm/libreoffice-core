/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: extract.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2006-01-10 15:51:47 $
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
#ifndef _COMPHELPER_EXTRACT_HXX_
#define _COMPHELPER_EXTRACT_HXX_

#ifndef _COM_SUN_STAR_LANG_ILLEGALARGUMENTEXCEPTION_HPP_
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_XINTERFACE_HPP_
#include <com/sun/star/uno/XInterface.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_TYPECLASS_HPP_
#include <com/sun/star/uno/TypeClass.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_TYPE_HXX_
#include <com/sun/star/uno/Type.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef INCLUDED_CPPU_UNOTYPE_HXX
#include "cppu/unotype.hxx"
#endif

namespace cppu
{

/**
 * Sets enum from int32 value.  This function does NOT check for valid enum values!
 *<BR>
 * @param nEnum         int32 enum value
 * @param rType         enum type
 * @return enum or emoty any.
 */
inline ::com::sun::star::uno::Any SAL_CALL int2enum(
    sal_Int32 nEnum, const ::com::sun::star::uno::Type & rType )
{
    if (rType.getTypeClass() == ::com::sun::star::uno::TypeClass_ENUM)
    {
        int nVal = nEnum;
        return ::com::sun::star::uno::Any( &nVal, rType );
    }
    return ::com::sun::star::uno::Any();
}

/**
 * Sets int32 from enum or int in any.
 *<BR>
 * @param rnEnum        [out] int32 enum value
 * @param rAny          enum or int
 * @param sal_True if enum or int value was set else sal_False.
 */
inline sal_Bool SAL_CALL enum2int( sal_Int32 & rnEnum, const ::com::sun::star::uno::Any & rAny )
{
    if (rAny.getValueTypeClass() == ::com::sun::star::uno::TypeClass_ENUM)
    {
        rnEnum = * reinterpret_cast< const int * >( rAny.getValue() );
        return sal_True;
    }

    return rAny >>= rnEnum;
}

/**
 * Sets int32 from enum or int in any with additional typecheck
 * <BR>
 * @param rAny          enum or int
 * @param eRet          the enum value as int. If there is not enum of the given type or
 *                      a ::com::sun::star::lang::IllegalArgumentException is thrown
 */
template< typename E >
inline void SAL_CALL any2enum( E & eRet, const ::com::sun::star::uno::Any & rAny )
    throw( ::com::sun::star::lang::IllegalArgumentException )
{
    // check for type save enum
    if (! (rAny >>= eRet))
    {
        // if not enum, maybe integer?
        sal_Int32 nValue;
        if (! (rAny >>= nValue))
            throw ::com::sun::star::lang::IllegalArgumentException();

        eRet = (E)nValue;
    }
}

/**
 * Template function to create an uno::Any from an enum
 *
 * @DEPRECATED : use makeAny< E >()
 *
 */
template< typename E >
inline ::com::sun::star::uno::Any SAL_CALL enum2any( E eEnum )
{
    return ::com::sun::star::uno::Any( &eEnum, ::cppu::UnoType< E >::get() );
}

/**
 * Extracts interface from an any.  If given any does not hold the demanded interface,
 * it will be queried for it.
 * If no interface is available, the out ref will be cleared.
 *<BR>
 * @param rxOut         [out] demanded interface
 * @param rAny          interface
 * @return sal_True if any reference (including the null ref) was retrieved from any else sal_False.
 */
template< class T >
inline sal_Bool SAL_CALL extractInterface(
    ::com::sun::star::uno::Reference< T > & rxOut,
    const ::com::sun::star::uno::Any & rAny )
{
    rxOut.clear();
    return (rAny >>= rxOut);
}

/**
 * extracts a boolean either as a sal_Bool or an integer from
 * an any. If there is no sal_Bool or integer inside the any
 * a ::com::sun::star::lang::IllegalArgumentException is thrown
 *
 */
inline sal_Bool SAL_CALL any2bool( const ::com::sun::star::uno::Any & rAny )
    throw( ::com::sun::star::lang::IllegalArgumentException )
{
    if (rAny.getValueTypeClass() == ::com::sun::star::uno::TypeClass_BOOLEAN)
    {
        return *(sal_Bool *)rAny.getValue();
    }
    else
    {
        sal_Int32 nValue;
        if (! (rAny >>= nValue))
            throw ::com::sun::star::lang::IllegalArgumentException();
        return nValue != 0;
    }
}

/**
 * Puts a boolean in an any.
 *
 * @DEPRECATED : use makeAny< sal_Bool >()
 *
 */
inline ::com::sun::star::uno::Any SAL_CALL bool2any( sal_Bool bBool )
{
    return ::com::sun::star::uno::Any( &bBool, ::getCppuBooleanType() );
}

}

#endif
