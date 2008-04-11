/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: vbaformatconditions.hxx,v $
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
#ifndef SC_VBA_FORMATCONDITIONS_HXX
#define SC_VBA_FORMATCONDITIONS_HXX
#include <org/openoffice/excel/XFormatConditions.hpp>
#include <org/openoffice/excel/XStyles.hpp>
#include <org/openoffice/excel/XRange.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/table/CellAddress.hpp>
#include <com/sun/star/sheet/XSheetConditionalEntries.hpp>
#include "vbacollectionimpl.hxx"

typedef CollTestImplHelper< oo::excel::XFormatConditions > ScVbaFormatConditions_BASE;
class ScVbaFormatConditions: public ScVbaFormatConditions_BASE
{
    css::table::CellAddress maCellAddress;
    css::uno::Reference< css::sheet::XSheetConditionalEntries > mxSheetConditionalEntries;
    css::uno::Reference< oo::excel::XStyles > mxStyles;
    css::uno::Reference< oo::excel::XRange > mxRangeParent;
    css::uno::Reference< css::beans::XPropertySet > mxParentRangePropertySet;
public:
    ScVbaFormatConditions( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext > & xContext, const css::uno::Reference< css::sheet::XSheetConditionalEntries >&, const css::uno::Reference< css::frame::XModel >&  );
    void notifyRange() throw ( css::script::BasicErrorException );
    virtual css::uno::Reference< oo::excel::XFormatCondition > Add( ::sal_Int32 Type, const css::uno::Any& Operator, const css::uno::Any& Formula1, const css::uno::Any& Formula2, const css::uno::Reference< oo::excel::XStyle >& _xCalcStyle ) throw (css::script::BasicErrorException, css::uno::RuntimeException);
    rtl::OUString getA1Formula(const css::uno::Any& _aFormula) throw ( css::script::BasicErrorException );
    rtl::OUString getStyleName();
    void removeFormatCondition( const rtl::OUString& _sStyleName, sal_Bool _bRemoveStyle) throw ( css::script::BasicErrorException );
    css::uno::Reference< css::sheet::XSheetConditionalEntries > getSheetConditionalEntries() { return mxSheetConditionalEntries; }
    // XFormatConditions
    virtual void SAL_CALL Delete(  ) throw (css::script::BasicErrorException, css::uno::RuntimeException);
    virtual css::uno::Reference< oo::excel::XFormatCondition > SAL_CALL Add( ::sal_Int32 Type, const css::uno::Any& Operator, const css::uno::Any& Formula1, const css::uno::Any& Formula2 ) throw (css::script::BasicErrorException, css::uno::RuntimeException);
    // XEnumerationAccess
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);
    virtual css::uno::Any createCollectionObject(const css::uno::Any&);
    // XHelperInterface
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif //SC_VBA_AXES_HXX
