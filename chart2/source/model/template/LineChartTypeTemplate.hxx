/*************************************************************************
 *
 *  $RCSfile: LineChartTypeTemplate.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: bm $ $Date: 2004-01-26 09:12:53 $
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
#ifndef CHART_LINECHARTTYPETEMPLATE_HXX
#define CHART_LINECHARTTYPETEMPLATE_HXX

#include "ChartTypeTemplate.hxx"
#include "OPropertySet.hxx"
#include "MutexContainer.hxx"

#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif

#ifndef _COM_SUN_STAR_CHART2_STACKMODE_HPP_
#include <com/sun/star/chart2/StackMode.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_CURVESTYLE_HPP_
#include <com/sun/star/chart2/CurveStyle.hpp>
#endif

namespace chart
{

class LineChartTypeTemplate :
        public helper::MutexContainer,
        public ChartTypeTemplate,
        public ::property::OPropertySet
{
public:
    explicit LineChartTypeTemplate(
        ::com::sun::star::uno::Reference<
            ::com::sun::star::uno::XComponentContext > const & xContext,
        const ::rtl::OUString & rServiceName,
        ::com::sun::star::chart2::StackMode eYStackMode,
        ::com::sun::star::chart2::CurveStyle eCurveStyle,
        bool bSymbols, sal_Int32 nDim = 2,
        ::com::sun::star::chart2::StackMode eZStackMode =
            ::com::sun::star::chart2::StackMode_NONE );
    virtual ~LineChartTypeTemplate();

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
    virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();

    // ____ XPropertySet ____
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL
        getPropertySetInfo()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XChartTypeTemplate ____
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XDiagram > SAL_CALL
        createDiagram( const ::com::sun::star::uno::Sequence<
                           ::com::sun::star::uno::Reference<
                               ::com::sun::star::chart2::XDataSeries > >& aSeriesSeq )
        throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL matchesTemplate(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::chart2::XDiagram >& xDiagram )
        throw (::com::sun::star::uno::RuntimeException);

    // ____ ChartTypeTemplate ____
    virtual sal_Int32 getDimension() const;
    virtual ::com::sun::star::chart2::StackMode getZStackMode() const;
    virtual ::com::sun::star::chart2::StackMode getYStackMode() const;
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::chart2::XChartType > getDefaultChartType()
        throw (::com::sun::star::uno::RuntimeException);

private:
    ::com::sun::star::chart2::StackMode
                       m_eYStackMode;
    ::com::sun::star::chart2::CurveStyle
                       m_eCurveStyle;
    bool               m_bHasSymbols;
    bool               m_bHasLines;
    sal_Int32          m_nDim;

    // only used if m_nDim == 3
    ::com::sun::star::chart2::StackMode
                       m_eZStackMode;
};

} //  namespace chart

// CHART_LINECHARTTYPETEMPLATE_HXX
#endif
