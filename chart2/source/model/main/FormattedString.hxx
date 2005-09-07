/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: FormattedString.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 00:59:48 $
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
#ifndef CHART_FORMATTEDSTRING_HXX
#define CHART_FORMATTEDSTRING_HXX

#ifndef CHART_MUTEXCONTAINER_HXX
#include "MutexContainer.hxx"
#endif
#ifndef CHART_OPROPERTYSET_HXX
#include "OPropertySet.hxx"
#endif

#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
#include "ServiceMacros.hxx"

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_XFORMATTEDSTRING_HPP_
#include <com/sun/star/chart2/XFormattedString.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_XCOMPONENTCONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif

namespace chart
{

namespace impl
{
typedef ::cppu::WeakImplHelper2<
    ::com::sun::star::chart2::XFormattedString,
    ::com::sun::star::lang::XServiceInfo
    >
    FormattedString_Base;
}

class FormattedString :
    public helper::MutexContainer,
    public impl::FormattedString_Base,
    public ::property::OPropertySet
{
public:
    FormattedString( const ::rtl::OUString & rString );
    FormattedString( ::com::sun::star::uno::Reference<
           ::com::sun::star::uno::XComponentContext > const & xContext );
    virtual ~FormattedString();

    /// declare XServiceInfo methods
    APPHELPER_XSERVICEINFO_DECL()
    /// establish methods for factory instatiation
    APPHELPER_SERVICE_FACTORY_HELPER( FormattedString )

    /// merge XInterface implementations
     DECLARE_XINTERFACE()
    /// merge XTypeProvider implementations
     DECLARE_XTYPEPROVIDER()

protected:
    // ____ XFormattedString ____
    virtual ::rtl::OUString SAL_CALL getString()
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setString( const ::rtl::OUString& String )
        throw (::com::sun::star::uno::RuntimeException);

    // ____ OPropertySet ____
    virtual ::com::sun::star::uno::Any GetDefaultValue( sal_Int32 nHandle ) const
        throw(::com::sun::star::beans::UnknownPropertyException);

    // ____ OPropertySet ____
    virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();

    // ____ XPropertySet ____
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL
        getPropertySetInfo()
        throw (::com::sun::star::uno::RuntimeException);

//  virtual sal_Bool SAL_CALL convertFastPropertyValue
//         ( ::com::sun::star::uno::Any & rConvertedValue,
//           ::com::sun::star::uno::Any & rOldValue,
//           sal_Int32 nHandle,
//           const ::com::sun::star::uno::Any& rValue )
//      throw (::com::sun::star::lang::IllegalArgumentException);

private:
    ::rtl::OUString m_aString;
};

} //  namespace chart

// CHART_FORMATTEDSTRING_HXX
#endif
