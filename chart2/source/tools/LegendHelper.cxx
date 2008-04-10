/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: LegendHelper.cxx,v $
 * $Revision: 1.9 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_chart2.hxx"
#include "LegendHelper.hxx"
#include "macros.hxx"
#include <com/sun/star/chart2/XChartDocument.hpp>
#include <com/sun/star/chart2/XLegend.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <tools/debug.hxx>

using namespace ::com::sun::star;

//.............................................................................
namespace chart
{
//.............................................................................

// static
uno::Reference< chart2::XLegend > LegendHelper::getLegend(
      const uno::Reference< frame::XModel >& xModel
    , const uno::Reference< uno::XComponentContext >& xContext
    , bool bCreate )
{
    uno::Reference< chart2::XLegend > xResult;

    uno::Reference< chart2::XChartDocument > xChartDoc( xModel, uno::UNO_QUERY );
    if( xChartDoc.is())
    {
        try
        {
            uno::Reference< chart2::XDiagram > xDia( xChartDoc->getFirstDiagram());
            if( xDia.is() )
            {
                xResult.set( xDia->getLegend() );
                if( bCreate && !xResult.is() && xContext.is() )
                {
                    xResult.set( xContext->getServiceManager()->createInstanceWithContext(
                        C2U( "com.sun.star.chart2.Legend" ), xContext ), uno::UNO_QUERY );
                    xDia->setLegend( xResult );
                }
            }
            else if(bCreate)
            {
                DBG_ERROR("need diagram for creation of legend");
            }
        }
        catch( uno::Exception & ex )
        {
            ASSERT_EXCEPTION( ex );
        }
    }

    return xResult;
}

// static
bool LegendHelper::hasLegend( const uno::Reference< chart2::XDiagram > & xDiagram )
{
    bool bReturn = false;
    if( xDiagram.is())
    {
        uno::Reference< beans::XPropertySet > xLegendProp( xDiagram->getLegend(), uno::UNO_QUERY );
        if( xLegendProp.is())
            xLegendProp->getPropertyValue( C2U("Show")) >>= bReturn;
    }

    return bReturn;
}

//.............................................................................
} //namespace chart
//.............................................................................

