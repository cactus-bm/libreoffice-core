/*************************************************************************
 *
 *  $RCSfile: DiagramWrapper.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: bm $ $Date: 2004-01-26 09:11:50 $
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
#ifndef CHART_DIAGRAMWRAPPER_HXX
#define CHART_DIAGRAMWRAPPER_HXX

#include "OPropertySet.hxx"
#include "ServiceMacros.hxx"

#ifndef _CPPUHELPER_IMPLBASE10_HXX_
#include <cppuhelper/implbase10.hxx>
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
#ifndef _CPPUHELPER_INTERFACECONTAINER_HXX_
#include <cppuhelper/interfacecontainer.hxx>
#endif

#ifndef _COM_SUN_STAR_CHART_XDIAGRAM_HPP_
#include <com/sun/star/chart/XDiagram.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART_XAXISZSUPPLIER_HPP_
#include <com/sun/star/chart/XAxisZSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART_XTWOAXISXSUPPLIER_HPP_
#include <com/sun/star/chart/XTwoAxisXSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART_XTWOAXISYSUPPLIER_HPP_
#include <com/sun/star/chart/XTwoAxisYSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART_XSTATISTICDISPLAY_HPP_
#include <com/sun/star/chart/XStatisticDisplay.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART_X3DDISPLAY_HPP_
#include <com/sun/star/chart/X3DDisplay.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XEVENTLISTENER_HPP_
#include <com/sun/star/lang/XEventListener.hpp>
#endif

#ifndef _COM_SUN_STAR_CHART2_XDIAGRAM_HPP_
#include <com/sun/star/chart2/XDiagram.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_XCOMPONENTCONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_XDIAGRAMPROVIDER_HPP_
#include <com/sun/star/chart2/XDiagramProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_XCHARTTYPETEMPLATE_HPP_
#include <com/sun/star/chart2/XChartTypeTemplate.hpp>
#endif

namespace com { namespace sun { namespace star {
namespace chart2
{
    class XTitle;
}
}}}

namespace chart
{
namespace wrapper
{

namespace impl
{
typedef ::cppu::WeakImplHelper10<
    ::com::sun::star::chart::XDiagram,
    ::com::sun::star::chart::XAxisZSupplier,
    ::com::sun::star::chart::XTwoAxisXSupplier, //  : XAxisXSupplier
    ::com::sun::star::chart::XTwoAxisYSupplier, //  : XAxisYSupplier
    ::com::sun::star::chart::XStatisticDisplay,
    ::com::sun::star::chart::X3DDisplay,
    ::com::sun::star::lang::XServiceInfo,
       ::com::sun::star::lang::XComponent,
    ::com::sun::star::lang::XEventListener,
    ::com::sun::star::chart2::XDiagramProvider >
    DiagramWrapper_Base;
}

class DiagramWrapper :
        public impl::DiagramWrapper_Base,
        public ::property::OPropertySet
{
public:

    DiagramWrapper( const ::com::sun::star::uno::Reference<
                        ::com::sun::star::chart2::XDiagram > & xDia,
                    const ::com::sun::star::uno::Reference<
                        ::com::sun::star::uno::XComponentContext > & xContext,
                    ::osl::Mutex & _rMutex );
    virtual ~DiagramWrapper();

    ::osl::Mutex & GetMutex() const;

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

//     virtual sal_Bool SAL_CALL convertFastPropertyValue
//         ( ::com::sun::star::uno::Any & rConvertedValue,
//           ::com::sun::star::uno::Any & rOldValue,
//           sal_Int32 nHandle,
//           const ::com::sun::star::uno::Any& rValue )
//      throw (::com::sun::star::lang::IllegalArgumentException);

    virtual void SAL_CALL getFastPropertyValue
        ( ::com::sun::star::uno::Any& rValue,
          sal_Int32 nHandle ) const;

    virtual void SAL_CALL setFastPropertyValue_NoBroadcast(
        sal_Int32 nHandle,
        const ::com::sun::star::uno::Any& rValue )
        throw (::com::sun::star::uno::Exception);

    // ____ XPropertySet ____
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL
        getPropertySetInfo()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XComponent ____
    virtual void SAL_CALL dispose()
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference<
                                            ::com::sun::star::lang::XEventListener >& xListener )
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference<
                                               ::com::sun::star::lang::XEventListener >& aListener )
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XDiagram ____
    virtual ::rtl::OUString SAL_CALL getDiagramType()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getDataRowProperties( sal_Int32 nRow )
        throw (::com::sun::star::lang::IndexOutOfBoundsException,
               ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getDataPointProperties( sal_Int32 nCol, sal_Int32 nRow )
        throw (::com::sun::star::lang::IndexOutOfBoundsException,
               ::com::sun::star::uno::RuntimeException);

    // ____ XShape (base of XDiagram) ____
    virtual ::com::sun::star::awt::Point SAL_CALL getPosition()
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPosition( const ::com::sun::star::awt::Point& aPosition )
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::awt::Size SAL_CALL getSize()
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setSize( const ::com::sun::star::awt::Size& aSize )
        throw (::com::sun::star::beans::PropertyVetoException,
               ::com::sun::star::uno::RuntimeException);

    // ____ XShapeDescriptor (base of XShape) ____
    virtual ::rtl::OUString SAL_CALL getShapeType()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XAxisZSupplier ____
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShape > SAL_CALL getZAxisTitle()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getZMainGrid()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getZHelpGrid()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getZAxis()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XTwoAxisXSupplier ____
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getSecondaryXAxis()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XAxisXSupplier (base of XTwoAxisXSupplier) ____
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShape > SAL_CALL getXAxisTitle()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getXAxis()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getXMainGrid()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getXHelpGrid()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XTwoAxisYSupplier ____
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getSecondaryYAxis()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XAxisYSupplier (base of XTwoAxisYSupplier) ____
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShape > SAL_CALL getYAxisTitle()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getYAxis()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getYHelpGrid()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getYMainGrid()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XStatisticDisplay ____
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getUpBar()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getDownBar()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getMinMaxLine()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ X3DDisplay ____
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getWall()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getFloor()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XEventListener ____
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source )
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XDiagramProvider ____
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::chart2::XDiagram > SAL_CALL getDiagram()
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setDiagram( const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::chart2::XDiagram >& xDiagram )
        throw (::com::sun::star::uno::RuntimeException);

private:
    mutable ::osl::Mutex &    m_rMutex;

    ::com::sun::star::uno::Reference<
        ::com::sun::star::uno::XComponentContext >
                        m_xContext;

    ::cppu::OInterfaceContainerHelper
                        m_aEventListenerContainer;

    ::com::sun::star::uno::Reference<
        ::com::sun::star::chart2::XDiagram >
                        m_xDiagram;

    ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShape >
                        m_xXAxisTitle;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShape >
                        m_xYAxisTitle;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShape >
                        m_xZAxisTitle;

    ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet >
                        m_xXAxis;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet >
                        m_xYAxis;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet >
                        m_xZAxis;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet >
                        m_xSecondXAxis;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet >
                        m_xSecondYAxis;
};

} //  namespace wrapper
} //  namespace chart

// CHART_DIAGRAMWRAPPER_HXX
#endif
