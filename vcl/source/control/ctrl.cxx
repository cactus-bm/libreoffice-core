/*************************************************************************
 *
 *  $RCSfile: ctrl.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: mt $ $Date: 2001-11-27 09:54:45 $
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
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#define _SV_CTRL_CXX

#ifndef _SV_RC_H
#include <rc.h>
#endif
#ifndef _SV_SVDATA_HXX
#include <svdata.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <svapp.hxx>
#endif
#ifndef _SV_EVENT_HXX
#include <event.hxx>
#endif
#ifndef _SV_CTRL_HXX
#include <ctrl.hxx>
#endif

#pragma hdrstop

// =======================================================================

void Control::ImplInitData()
{
    mbHasFocus = FALSE;
}

// -----------------------------------------------------------------------

Control::Control( WindowType nType ) :
    Window( nType )
{
    ImplInitData();
}

// -----------------------------------------------------------------------

Control::Control( Window* pParent, WinBits nStyle ) :
    Window( WINDOW_CONTROL )
{
    ImplInitData();
    Window::ImplInit( pParent, nStyle, NULL );
}

// -----------------------------------------------------------------------

Control::Control( Window* pParent, const ResId& rResId ) :
    Window( WINDOW_CONTROL )
{
    ImplInitData();
    rResId.SetRT( RSC_CONTROL );
    WinBits nStyle = ImplInitRes( rResId );
    ImplInit( pParent, nStyle, NULL );
    ImplLoadRes( rResId );

    if ( !(nStyle & WB_HIDE) )
        Show();
}

// -----------------------------------------------------------------------

void Control::GetFocus()
{
    Window::GetFocus();
}

// -----------------------------------------------------------------------

void Control::LoseFocus()
{
    Window::LoseFocus();
}

// -----------------------------------------------------------------------

long Control::Notify( NotifyEvent& rNEvt )
{
    if ( rNEvt.GetType() == EVENT_GETFOCUS )
    {
        if ( !mbHasFocus )
        {
            mbHasFocus = TRUE;
            ImplDelData aDelData;
            ImplAddDel( &aDelData );
            ImplCallEventListeners( VCLEVENT_CONTROL_GETFOCUS );
            if ( aDelData.IsDelete() )
                return TRUE;
            maGetFocusHdl.Call( this );
            if ( aDelData.IsDelete() )
                return TRUE;
            ImplRemoveDel( &aDelData );
        }
    }
    else
    {
        if ( rNEvt.GetType() == EVENT_LOSEFOCUS )
        {
            Window* pFocusWin = Application::GetFocusWindow();
            if ( !pFocusWin || !ImplIsWindowOrChild( pFocusWin ) )
            {
                mbHasFocus = FALSE;
                ImplDelData aDelData;
                ImplAddDel( &aDelData );
                ImplCallEventListeners( VCLEVENT_CONTROL_LOSEFOCUS );
                if ( aDelData.IsDelete() )
                    return TRUE;
                maLoseFocusHdl.Call( this );
                if ( aDelData.IsDelete() )
                    return TRUE;
                ImplRemoveDel( &aDelData );
            }
        }
    }

    return Window::Notify( rNEvt );
}
