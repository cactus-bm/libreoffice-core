/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: GridWrapper.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2007-09-18 14:52:46 $
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
#ifndef CHART_GRIDWRAPPER_HXX
#define CHART_GRIDWRAPPER_HXX

#include "WrappedPropertySet.hxx"
#include "ServiceMacros.hxx"

#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
#ifndef _CPPUHELPER_INTERFACECONTAINER_HXX_
#include <cppuhelper/interfacecontainer.hxx>
#endif

#ifndef _COM_SUN_STAR_DRAWING_XSHAPE_HPP_
#include <com/sun/star/drawing/XShape.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef _COM_SUN_STAR_CHART2_XDIAGRAM_HPP_
#include <com/sun/star/chart2/XDiagram.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_XCOMPONENTCONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif

#include <boost/shared_ptr.hpp>

namespace chart
{

namespace wrapper
{

class Chart2ModelContact;

class GridWrapper : public ::cppu::ImplInheritanceHelper2<
                      WrappedPropertySet
                    , com::sun::star::lang::XComponent
                    , com::sun::star::lang::XServiceInfo
                    >
{
public:
    enum tGridType
    {
        X_MAIN_GRID,
        Y_MAIN_GRID,
        Z_MAIN_GRID,
        X_SUB_GRID,
        Y_SUB_GRID,
        Z_SUB_GRID
    };

    GridWrapper( tGridType eType, ::boost::shared_ptr< Chart2ModelContact > spChart2ModelContact );
    virtual ~GridWrapper();

    static void getDimensionAndSubGridBool( tGridType eType, sal_Int32& rnDimensionIndex, bool& rbSubGrid );

    /// XServiceInfo declarations
    APPHELPER_XSERVICEINFO_DECL()

    // ____ XComponent ____
    virtual void SAL_CALL dispose()
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference<
                                            ::com::sun::star::lang::XEventListener >& xListener )
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference<
                                               ::com::sun::star::lang::XEventListener >& aListener )
        throw (::com::sun::star::uno::RuntimeException);

protected:
    // ____ WrappedPropertySet ____
    virtual const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& getPropertySequence();
    virtual const std::vector< WrappedProperty* > createWrappedProperties();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > getInnerPropertySet();

private:
    ::boost::shared_ptr< Chart2ModelContact >   m_spChart2ModelContact;
    ::cppu::OInterfaceContainerHelper           m_aEventListenerContainer;

    tGridType           m_eType;
};

} //  namespace wrapper
} //  namespace chart

// CHART_GRIDWRAPPER_HXX
#endif
