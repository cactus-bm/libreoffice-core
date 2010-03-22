/*************************************************************************
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* Copyright 2009 by Sun Microsystems, Inc.
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
************************************************************************/

#ifndef SVTOOLS_SOURCE_TABLE_TABLEDATAWINDOW_HXX
#define SVTOOLS_SOURCE_TABLE_TABLEDATAWINDOW_HXX

#ifndef _SV_WINDOW_HXX
#include <vcl/window.hxx>
#endif
#include <vcl/seleng.hxx>

//........................................................................
namespace svt { namespace table
{
//........................................................................

    class TableControl_Impl;
    class TableFunctionSet;


    //====================================================================
    //= TableDataWindow
    //====================================================================
    /** the window containing the content area (including headers) of
        a table control
    */
    class TableDataWindow : public Window
    {
        friend class TableFunctionSet;
    private:
        TableControl_Impl&  m_rTableControl;
        Link                m_aSelectHdl;
    public:
        TableDataWindow( TableControl_Impl& _rTableControl );
        inline void         SetSelectHdl( const Link& rLink )   { m_aSelectHdl = rLink; }
        inline const Link&  GetSelectHdl() const                { return m_aSelectHdl; }

        // Window overridables
        virtual void        Paint( const Rectangle& rRect );
        virtual void        MouseMove( const MouseEvent& rMEvt);
        virtual void        MouseButtonDown( const MouseEvent& rMEvt);
        virtual void        MouseButtonUp( const MouseEvent& rMEvt);
        virtual void        SetPointer( const Pointer& rPointer );
        virtual void        CaptureMouse();
        virtual void        ReleaseMouse();

    };
//........................................................................
} } // namespace svt::table
//........................................................................

#endif // SVTOOLS_SOURCE_TABLE_TABLEDATAWINDOW_HXX
