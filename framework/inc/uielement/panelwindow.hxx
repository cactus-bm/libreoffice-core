/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
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

#ifndef __FRAMEWORK_LAYOUTMANAGER_PANELWINDOW_HXX_
#define __FRAMEWORK_LAYOUTMANAGER_PANELWINDOW_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#include <vcl/dockwin.hxx>

//_________________________________________________________________________________________________________________
//  namespace
//_________________________________________________________________________________________________________________

namespace framework
{

class PanelWindow : public DockingWindow
{
    public:
                        PanelWindow( Window* pParent, WinBits nWinBits  =0);
        virtual         ~PanelWindow();

        virtual void    Command ( const CommandEvent& rCEvt );
        virtual void    StateChanged( StateChangedType nType );
        virtual void    DataChanged( const DataChangedEvent& rDCEvt );
        virtual void    Resize();

        // Provide additional handlers to support external implementations
        void            SetCommandHdl( const Link& aLink ) { m_aCommandHandler = aLink; }
        const Link&     GetCommandHdl() const { return m_aCommandHandler; }
        void            SetStateChangedHdl( const Link& aLink ) { m_aStateChangedHandler = aLink; }
        const Link&     GetStateChangedHdl() const { return m_aStateChangedHandler; }
        void            SetDataChangedHdl( const Link& aLink ) { m_aDataChangedHandler = aLink; }
        const Link&     GetDataChangedHdl() { return m_aDataChangedHandler; }

    private:
        ::rtl::OUString m_aResourceURL;
        Link            m_aCommandHandler;
        Link            m_aStateChangedHandler;
        Link            m_aDataChangedHandler;
        Window*         m_pContentWindow;
};

}

#endif // __FRAMEWORK_UIELEMENT_PANELWINDOW_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
