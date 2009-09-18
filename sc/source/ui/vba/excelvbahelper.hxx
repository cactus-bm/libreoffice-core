/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: vbahelper.hxx,v $
 * $Revision: 1.5.32.1 $
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
#ifndef SC_EXCEL_VBA_HELPER_HXX
#define SC_EXCEL_VBA_HELPER_HXX

#include<vbahelper/vbahelper.hxx>
#include <docsh.hxx>

class ScCellRangeObj;

namespace ooo
{
    namespace vba
    {
    namespace excel
        {
                // nTabs empty means apply zoom to all sheets
                void implSetZoom( const css::uno::Reference< css::frame::XModel >& xModel, sal_Int16 nZoom, std::vector< SCTAB >& nTabs );
        void implnCopy( const css::uno::Reference< css::frame::XModel>& xModel );
        void implnPaste ( const css::uno::Reference< css::frame::XModel>& xModel );
        void implnCut( const css::uno::Reference< css::frame::XModel>& xModel );
        void implnPasteSpecial( const css::uno::Reference< css::frame::XModel>& xModel, sal_uInt16 nFlags,sal_uInt16 nFunction,sal_Bool bSkipEmpty, sal_Bool bTranspose);
        ScTabViewShell* getBestViewShell( const css::uno::Reference< css::frame::XModel>& xModel ) ;
        ScDocShell* getDocShell( const css::uno::Reference< css::frame::XModel>& xModel ) ;
        ScTabViewShell* getCurrentBestViewShell( const css::uno::Reference< css::uno::XComponentContext >& xContext );
        SfxViewFrame* getViewFrame( const css::uno::Reference< css::frame::XModel >& xModel );
            class ScVbaCellRangeAccess
            {
            public:
                static SfxItemSet* GetDataSet( ScCellRangeObj* pRangeObj );
            };
};
};
};
#endif
