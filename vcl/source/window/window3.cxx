/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: window3.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 12:33:18 $
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

#define _SV_WINDOW_CXX

#ifndef _SV_SVSYS_HXX
#include <svsys.h>
#endif

#ifndef _SV_WINDOW_H
#include <window.h>
#endif
#ifndef _SV_WINDOW_HXX
#include <window.hxx>
#endif
#ifndef _SV_WAITOBJ_HXX
#include <waitobj.hxx>
#endif

#ifndef _SV_SALGDI_HXX
#include <salgdi.hxx>
#endif
#ifndef _SV_NATIVEWIDGETS_HXX
#include <salnativewidgets.hxx>
#endif
#ifndef _SV_SALCTRLHANDLE_HXX
#include <salctrlhandle.hxx>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

using namespace rtl;

// -----------------------------------------------------------------------
// These functions are mainly passthrough functions that allow access to
// the SalFrame behind a Window object for native widget rendering purposes.
// -----------------------------------------------------------------------

void Window::ImplInitSalControlHandle()
{
    // create SalControlHandle on demand
    // not needed for ordinary windows
    // TODO: move creation to SalGraphics
    //if( !ImplGetWinData()->mpSalControlHandle )
    //    ImplGetWinData()->mpSalControlHandle = new SalControlHandle;
}

// -----------------------------------------------------------------------

BOOL Window::IsNativeControlSupported( ControlType nType, ControlPart nPart )
{
    if( !IsNativeWidgetEnabled() )
        return FALSE;

    if ( !mpGraphics )
        if ( !ImplGetGraphics() )
            return FALSE;

    return( mpGraphics->IsNativeControlSupported(nType, nPart) );
}


// -----------------------------------------------------------------------

BOOL Window::HitTestNativeControl( ControlType nType,
                              ControlPart nPart,
                              const Region& rControlRegion,
                              const Point& aPos,
                              BOOL& rIsInside )
{
    if( !IsNativeWidgetEnabled() )
        return FALSE;

    if ( !mpGraphics )
        if ( !ImplGetGraphics() )
            return FALSE;

    ImplInitSalControlHandle();
    return( mpGraphics->HitTestNativeControl(nType, nPart, rControlRegion, aPos, *ImplGetWinData()->mpSalControlHandle, rIsInside, this ) );
}

// -----------------------------------------------------------------------

BOOL Window::DrawNativeControl( ControlType nType,
                            ControlPart nPart,
                            const Region& rControlRegion,
                            ControlState nState,
                            const ImplControlValue& aValue,
                            OUString aCaption )
{
    if( !IsNativeWidgetEnabled() )
        return FALSE;

    /*
    if( !IsInPaint() && IsPaintTransparent() )
    {
        // only required if called directly (ie, we're not in Paint() ):
        // force redraw (Paint()) for transparent controls
        // to trigger a repaint of the background
        Region aClipRgn( GetClipRegion() );
        if( !rControlRegion.IsEmpty() )
            aClipRgn.Intersect( rControlRegion );
        Invalidate( aClipRgn, INVALIDATE_UPDATE );
        return TRUE;
    }
    */

    ImplInitSalControlHandle();

    // make sure the current clip region is initialized correctly
    if ( !mpGraphics )
        if ( !ImplGetGraphics() )
            return FALSE;

    if ( mbInitClipRegion )
        ImplInitClipRegion();
    if ( mbOutputClipped )
        return TRUE;

    if ( mbInitLineColor )
        ImplInitLineColor();
    if ( mbInitFillColor )
        ImplInitFillColor();

    // Convert the coordinates from relative to Window-absolute, so we draw
    // in the correct place in platform code
    Point aWinOffs;
    aWinOffs = OutputToScreenPixel( aWinOffs );
    Region screenRegion( rControlRegion );
    screenRegion.Move( aWinOffs.X(), aWinOffs.Y());

    Region aTestRegion( GetActiveClipRegion() );
    aTestRegion.Intersect( rControlRegion );
    if( aTestRegion == rControlRegion )
        nState |= CTRL_CACHING_ALLOWED;   // control is not clipped, caching allowed

    return( mpGraphics->DrawNativeControl(nType, nPart, screenRegion, nState, aValue, *ImplGetWinData()->mpSalControlHandle, aCaption, this ) );
}


// -----------------------------------------------------------------------

BOOL Window::DrawNativeControlText(ControlType nType,
                            ControlPart nPart,
                            const Region& rControlRegion,
                            ControlState nState,
                            const ImplControlValue& aValue,
                            OUString aCaption )
{
    if( !IsNativeWidgetEnabled() )
        return FALSE;

    ImplInitSalControlHandle();

    // make sure the current clip region is initialized correctly
    if ( !mpGraphics )
        if ( !ImplGetGraphics() )
            return false;

    if ( mbInitClipRegion )
        ImplInitClipRegion();
    if ( mbOutputClipped )
        return true;

    if ( mbInitLineColor )
        ImplInitLineColor();
    if ( mbInitFillColor )
        ImplInitFillColor();

    // Convert the coordinates from relative to Window-absolute, so we draw
    // in the correct place in platform code
    Point aWinOffs;
    aWinOffs = OutputToScreenPixel( aWinOffs );
    Region screenRegion( rControlRegion );
    screenRegion.Move( aWinOffs.X(), aWinOffs.Y());

    return( mpGraphics->DrawNativeControlText(nType, nPart, screenRegion, nState, aValue, *ImplGetWinData()->mpSalControlHandle, aCaption, this ) );
}


// -----------------------------------------------------------------------

BOOL Window::GetNativeControlRegion(  ControlType nType,
                                ControlPart nPart,
                                const Region& rControlRegion,
                                ControlState nState,
                                const ImplControlValue& aValue,
                                OUString aCaption,
                                Region &rNativeBoundingRegion,
                                Region &rNativeContentRegion )
{
    if( !IsNativeWidgetEnabled() )
        return FALSE;

    if ( !mpGraphics )
        if ( !ImplGetGraphics() )
            return FALSE;

    ImplInitSalControlHandle();
    return( mpGraphics->GetNativeControlRegion(nType, nPart, rControlRegion, nState, aValue,
                                *ImplGetWinData()->mpSalControlHandle, aCaption, rNativeBoundingRegion,
                                rNativeContentRegion, this ) );
}


// -----------------------------------------------------------------------

WaitObject::~WaitObject()
{
    if ( mpWindow )
        mpWindow->LeaveWait();
}
