/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: vbaaxes.hxx,v $
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
#ifndef SC_VBA_AXES_HXX
#define SC_VBA_AXES_HXX
#include <org/openoffice/excel/XAxes.hpp>
#include <org/openoffice/excel/XAxis.hpp>
#include <org/openoffice/excel/XChart.hpp>
#include "vbacollectionimpl.hxx"

typedef CollTestImplHelper< oo::excel::XAxes > ScVbaAxes_BASE;
class ScVbaAxes : public ScVbaAxes_BASE
{
    css::uno::Reference< oo::excel::XChart > moChartParent; // not the true parent I guess
public:
    ScVbaAxes( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext > & xContext, const css::uno::Reference< oo::excel::XChart >& xChart );
    // XEnumerationAccess
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);
    css::uno::Any SAL_CALL Item( const css::uno::Any& aIndex, const css::uno::Any& aIndex2 ) throw (css::uno::RuntimeException);
    virtual css::uno::Any createCollectionObject(const css::uno::Any&);
    // XHelperInterface
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
    static css::uno::Reference< oo::excel::XAxis > createAxis( const css::uno::Reference< oo::excel::XChart >& xChart, const css::uno::Reference< css::uno::XComponentContext >& xContext,  sal_Int32 nType, sal_Int32 nAxisGroup ) throw ( css::uno::RuntimeException );
};

#endif //SC_VBA_AXES_HXX
