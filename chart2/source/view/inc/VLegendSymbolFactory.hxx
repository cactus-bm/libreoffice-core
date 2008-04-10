/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: VLegendSymbolFactory.hxx,v $
 * $Revision: 1.3 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef CHART2_VLEGENDSYMBOLFACTORY_HXX
#define CHART2_VLEGENDSYMBOLFACTORY_HXX

#include <com/sun/star/chart2/LegendSymbolStyle.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/drawing/XShapes.hpp>

namespace chart
{

class VLegendSymbolFactory
{
public:
    enum tPropertyType
    {
        PROP_TYPE_FILLED_SERIES,
        PROP_TYPE_LINE_SERIES,
        PROP_TYPE_FILL,
        PROP_TYPE_LINE,
        PROP_TYPE_FILL_AND_LINE
    };

    static ::com::sun::star::uno::Reference<
                ::com::sun::star::drawing::XShape >
        createSymbol(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::drawing::XShapes > xSymbolContainer,
            ::com::sun::star::chart2::LegendSymbolStyle eStyle,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::lang::XMultiServiceFactory > & xShapeFactory,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XPropertySet > & xLegendEntryProperties,
            tPropertyType ePropertyType,
            const ::com::sun::star::uno::Any& rExplicitSymbol /*should contain a ::com::sun::star::chart2::Symbol without automatic symbol if the charttype does support symbols else empty*/);

    /// @param bWhiteDay: if <FALSE/> this symbol is for BlackDays
    static ::com::sun::star::uno::Reference<
                ::com::sun::star::drawing::XShape >
        createJapaneseCandleStickSymbol(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::drawing::XShapes > xSymbolContainer,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::lang::XMultiServiceFactory > & xShapeFactory,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XPropertySet > & xLegendEntryProperties,
            bool bWhiteDay );

    enum tStockLineType
    {
        STOCK_LINE_TYPE_VERT,
        STOCK_LINE_TYPE_OPEN,
        STOCK_LINE_TYPE_CLOSE
    };

    static ::com::sun::star::uno::Reference<
                ::com::sun::star::drawing::XShape >
        createStockLineSymbol(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::drawing::XShapes > xSymbolContainer,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::lang::XMultiServiceFactory > & xShapeFactory,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XPropertySet > & xLegendEntryProperties,
            tStockLineType eType );

private:
     VLegendSymbolFactory();
};

} //  namespace chart

// CHART2_VLEGENDSYMBOLFACTORY_HXX
#endif
