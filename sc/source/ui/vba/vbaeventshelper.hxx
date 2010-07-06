/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#ifndef SC_VBAEVENTS_HXX
#define SC_VBAEVENTS_HXX

#include <rtl/ref.hxx>
#include <vbahelper/vbaeventshelperbase.hxx>
#include "excelvbahelper.hxx"
#include "rangelst.hxx"

namespace ooo { namespace vba { namespace excel { class XApplication; } } }

class ScVbaWindowListener;

// ============================================================================

class ScVbaEventsHelper : public VbaEventsHelperBase
{
public:
    ScVbaEventsHelper(
        const css::uno::Sequence< css::uno::Any >& rArgs,
        const css::uno::Reference< css::uno::XComponentContext >& rxContext );
    virtual ~ScVbaEventsHelper();

protected:
    virtual bool implEventsEnabled() throw (css::uno::RuntimeException);
    virtual bool implPrepareEvent( EventQueue& rEventQueue, const EventHandlerInfo& rInfo, const css::uno::Sequence< css::uno::Any >& rArgs ) throw (css::uno::RuntimeException);
    virtual css::uno::Sequence< css::uno::Any > implBuildArgumentList( const EventHandlerInfo& rInfo, const css::uno::Sequence< css::uno::Any >& rArgs ) throw (css::lang::IllegalArgumentException);
    virtual void implPostProcessEvent( EventQueue& rEventQueue, const EventHandlerInfo& rInfo, bool bSuccess, bool bCancel ) throw (css::uno::RuntimeException);
    virtual ::rtl::OUString implGetDocumentModuleName( const EventHandlerInfo& rInfo, const css::uno::Sequence< css::uno::Any >& rArgs ) const throw (css::lang::IllegalArgumentException);

private:
    /** Extracts a sheet index from the first element of the passed sequence. The
        element may be an integer, or a Calc range or ranges object. */
    static SCTAB getTabFromArgs( const css::uno::Sequence< css::uno::Any >& rArgs, sal_Int32 nIndex ) throw (css::lang::IllegalArgumentException);
    /** Checks if selection has been changed compared to selection of last call.
        @return true, if the selection has been changed. */
    bool isSelectionChanged( const css::uno::Sequence< css::uno::Any >& rArgs, sal_Int32 nIndex ) throw (css::lang::IllegalArgumentException, css::uno::RuntimeException);

    /** Creates a VBA Worksheet object (the argument must contain a sheet index). */
    css::uno::Any createWorksheet( const css::uno::Sequence< css::uno::Any >& rArgs, sal_Int32 nIndex ) const throw (css::lang::IllegalArgumentException, css::uno::RuntimeException);
    /** Creates a VBA Range object (the argument must contain a UNO range or UNO range list). */
    css::uno::Any createRange( const css::uno::Sequence< css::uno::Any >& rArgs, sal_Int32 nIndex ) const throw (css::lang::IllegalArgumentException, css::uno::RuntimeException);
    /** Creates a VBA Hyperlink object (the argument must contain a UNO cell). */
    css::uno::Any createHyperlink( const css::uno::Sequence< css::uno::Any >& rArgs, sal_Int32 nIndex ) const throw (css::lang::IllegalArgumentException, css::uno::RuntimeException);
    /** Creates a VBA Window object. */
    css::uno::Any createWindow() const throw (css::uno::RuntimeException);

private:
    mutable css::uno::WeakReference< ov::excel::XApplication > mxApplication;
    ::rtl::Reference< ScVbaWindowListener > mxWindowListener;
    ScDocShell* mpDocShell;
    ScDocument* mpDoc;
    ScRangeList maOldSelection;
    bool mbOpened;
    bool mbClosed;
};

// ============================================================================

#endif
