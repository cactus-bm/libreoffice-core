/*************************************************************************
 *
 *  $RCSfile: RegressionCurveModel.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: bm $ $Date: 2004-01-26 09:13:05 $
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
 *  Copyright: 2003 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/
#ifndef CHART2_REGRESSIONCURVEMODEL_HXX
#define CHART2_REGRESSIONCURVEMODEL_HXX

#include "MutexContainer.hxx"
#include "OPropertySet.hxx"
#include "ServiceMacros.hxx"

#ifndef _CPPUHELPER_IMPLBASE4_HXX_
#include <cppuhelper/implbase4.hxx>
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif

#ifndef _COM_SUN_STAR_CHART2_XIDENTIFIABLE_HPP_
#include <com/sun/star/chart2/XIdentifiable.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_XREGRESSIONCURVE_HPP_
#include <com/sun/star/chart2/XRegressionCurve.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_XCOMPONENTCONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICENAME_HPP_
#include <com/sun/star/lang/XServiceName.hpp>
#endif

namespace chart
{

namespace impl
{
typedef ::cppu::WeakImplHelper4<
        ::com::sun::star::lang::XServiceInfo,
        ::com::sun::star::lang::XServiceName,
        ::com::sun::star::chart2::XRegressionCurve,
        ::com::sun::star::chart2::XIdentifiable >
    RegressionCurveModel_Base;
}

class RegressionCurveModel :
        public helper::MutexContainer,
        public impl::RegressionCurveModel_Base,
        public ::property::OPropertySet
{
public:
    enum tCurveType
    {
        CURVE_TYPE_MEAN_VALUE,
        CURVE_TYPE_LINEAR,
        CURVE_TYPE_LOGARITHM,
        CURVE_TYPE_EXPONENTIAL,
        CURVE_TYPE_POWER,
    };

    RegressionCurveModel( ::com::sun::star::uno::Reference<
                              ::com::sun::star::uno::XComponentContext > const & xContext,
                          tCurveType eCurveType );
    virtual ~RegressionCurveModel();

    /// XServiceInfo declarations
    APPHELPER_XSERVICEINFO_DECL()

    /// merge XInterface implementations
     DECLARE_XINTERFACE()
    /// merge XTypeProvider implementations
     DECLARE_XTYPEPROVIDER()

protected:
    // ____ OPropertySet ____
    virtual ::com::sun::star::uno::Any GetDefaultValue( sal_Int32 nHandle ) const
        throw(::com::sun::star::beans::UnknownPropertyException);

    // ____ OPropertySet ____
    virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();

    // ____ XPropertySet ____
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL
        getPropertySetInfo()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XRegressionCurve ____
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::chart2::XRegressionCurveCalculator > SAL_CALL getCalculator()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XServiceName ____
    virtual ::rtl::OUString SAL_CALL getServiceName()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XIdentifiable ____
    virtual ::rtl::OUString SAL_CALL getIdentifier()
        throw (::com::sun::star::uno::RuntimeException);

private:
    ::com::sun::star::uno::Reference<
        ::com::sun::star::uno::XComponentContext >
                        m_xContext;

    ::rtl::OUString     m_aIdentifier;
    const tCurveType    m_eRegressionCurveType;
};

} //  namespace chart

// CHART2_REGRESSIONCURVEMODEL_HXX
#endif
