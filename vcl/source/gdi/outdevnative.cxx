/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_vcl.hxx"

#include "vcl/outdev.hxx"
#include "vcl/window.hxx"
#include "vcl/salgdi.hxx"
#include "vcl/salnativewidgets.hxx"
#include "vcl/pdfextoutdevdata.hxx"

// -----------------------------------------------------------------------

static bool lcl_enableNativeWidget( const OutputDevice& i_rDevice )
{
    const OutDevType eType( i_rDevice.GetOutDevType() );
    switch ( eType )
    {

    case OUTDEV_WINDOW:
        return dynamic_cast< const Window* >( &i_rDevice )->IsNativeWidgetEnabled();

    case OUTDEV_VIRDEV:
    {
        const ::vcl::ExtOutDevData* pOutDevData( i_rDevice.GetExtOutDevData() );
        const ::vcl::PDFExtOutDevData* pPDFData( dynamic_cast< const ::vcl::PDFExtOutDevData* >( pOutDevData ) );
        if ( pPDFData != NULL )
            return false;
        return true;
    }

    default:
        return false;
    }
}

ImplControlValue::~ImplControlValue()
{
}

ScrollbarValue::~ScrollbarValue()
{
}

SliderValue::~SliderValue()
{
}

TabitemValue::~TabitemValue()
{
}

SpinbuttonValue::~SpinbuttonValue()
{
}

ToolbarValue::~ToolbarValue()
{
}

MenubarValue::~MenubarValue()
{
}

PushButtonValue::~PushButtonValue()
{
}

// -----------------------------------------------------------------------
// These functions are mainly passthrough functions that allow access to
// the SalFrame behind a Window object for native widget rendering purposes.
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------

BOOL OutputDevice::IsNativeControlSupported( ControlType nType, ControlPart nPart )
{
    if( !lcl_enableNativeWidget( *this ) )
        return FALSE;

    if ( !mpGraphics )
        if ( !ImplGetGraphics() )
            return FALSE;

    return( mpGraphics->IsNativeControlSupported(nType, nPart) );
}


// -----------------------------------------------------------------------

BOOL OutputDevice::HitTestNativeControl( ControlType nType,
                              ControlPart nPart,
                              const Rectangle& rControlRegion,
                              const Point& aPos,
                              BOOL& rIsInside )
{
    if( !lcl_enableNativeWidget( *this ) )
        return FALSE;

    if ( !mpGraphics )
        if ( !ImplGetGraphics() )
            return FALSE;

    Point aWinOffs( mnOutOffX, mnOutOffY );
    Rectangle screenRegion( rControlRegion );
    screenRegion.Move( aWinOffs.X(), aWinOffs.Y());

    return( mpGraphics->HitTestNativeControl(nType, nPart, screenRegion, Point( aPos.X() + mnOutOffX, aPos.Y() + mnOutOffY ),
        rIsInside, this ) );
}

// -----------------------------------------------------------------------

static boost::shared_ptr< ImplControlValue > lcl_transformControlValue( const ImplControlValue& rVal, OutputDevice& rDev )
{
    boost::shared_ptr< ImplControlValue > aResult;
    switch( rVal.getType() )
    {
    case CTRL_SLIDER:
        {
            const SliderValue* pSlVal = static_cast<const SliderValue*>(&rVal);
            SliderValue* pNew = new SliderValue( *pSlVal );
            aResult.reset( pNew );
            pNew->maThumbRect = rDev.ImplLogicToDevicePixel( pSlVal->maThumbRect );
        }
        break;
    case CTRL_SCROLLBAR:
        {
            const ScrollbarValue* pScVal = static_cast<const ScrollbarValue*>(&rVal);
            ScrollbarValue* pNew = new ScrollbarValue( *pScVal );
            aResult.reset( pNew );
            pNew->maThumbRect = rDev.ImplLogicToDevicePixel( pScVal->maThumbRect );
            pNew->maButton1Rect = rDev.ImplLogicToDevicePixel( pScVal->maButton1Rect );
            pNew->maButton2Rect = rDev.ImplLogicToDevicePixel( pScVal->maButton2Rect );
        }
        break;
    case CTRL_SPINBUTTONS:
        {
            const SpinbuttonValue* pSpVal = static_cast<const SpinbuttonValue*>(&rVal);
            SpinbuttonValue* pNew = new SpinbuttonValue( *pSpVal );
            aResult.reset( pNew );
            pNew->maUpperRect = rDev.ImplLogicToDevicePixel( pSpVal->maUpperRect );
            pNew->maLowerRect = rDev.ImplLogicToDevicePixel( pSpVal->maLowerRect );
        }
        break;
    case CTRL_TOOLBAR:
        {
            const ToolbarValue* pTVal = static_cast<const ToolbarValue*>(&rVal);
            ToolbarValue* pNew = new ToolbarValue( *pTVal );
            aResult.reset( pNew );
            pNew->maGripRect = rDev.ImplLogicToDevicePixel( pTVal->maGripRect );
        }
        break;
    case CTRL_TAB_ITEM:
        {
            const TabitemValue* pTIVal = static_cast<const TabitemValue*>(&rVal);
            TabitemValue* pNew = new TabitemValue( *pTIVal );
            aResult.reset( pNew );
        }
        break;
    case CTRL_MENUBAR:
        {
            const MenubarValue* pMVal = static_cast<const MenubarValue*>(&rVal);
            MenubarValue* pNew = new MenubarValue( *pMVal );
            aResult.reset( pNew );
        }
        break;
    case CTRL_PUSHBUTTON:
        {
            const PushButtonValue* pBVal = static_cast<const PushButtonValue*>(&rVal);
            PushButtonValue* pNew = new PushButtonValue( *pBVal );
            aResult.reset( pNew );
        }
        break;
    case CTRL_GENERIC:
            aResult.reset( new ImplControlValue( rVal ) );
            break;
    default:
        OSL_ENSURE( 0, "unknown ImplControlValue type !" );
        break;
    }
    return aResult;
}

#if 0
static void lcl_moveControlValue( ControlType nType, const ImplControlValue& aValue, const Point& rDelta )
{
    switch( aValue.getType() )
    {
        case CTRL_SLIDER:
        {
            SliderValue* pSlVal = static_cast<SliderValue*>(const_cast<ImplControlValue*>(&aValue));
            pSlVal->maThumbRect.Move( rDelta.X(), rDelta.Y() );
        }
        break;
        case CTRL_SCROLLBAR:
        {
            ScrollbarValue* pScVal = static_cast<ScrollbarValue*>(const_cast<ImplControlValue*>(&aValue));
            pScVal->maThumbRect.Move( rDelta.X(), rDelta.Y() );
            pScVal->maButton1Rect.Move( rDelta.X(), rDelta.Y() );
            pScVal->maButton2Rect.Move( rDelta.X(), rDelta.Y() );
        }
        break;
        case CTRL_SPINBOX:
        case CTRL_SPINBUTTONS:
        {
            SpinbuttonValue* pSpVal = static_cast<SpinbuttonValue*>(const_cast<ImplControlValue*>(&aValue));
            pSpVal->maUpperRect.Move( rDelta.X(), rDelta.Y() );
            pSpVal->maLowerRect.Move( rDelta.X(), rDelta.Y() );
        }
        break;
        case CTRL_TOOLBAR:
        {
            ToolbarValue* pTVal = static_cast<ToolbarValue*>(const_cast<ImplControlValue*>(&aValue));
            pTVal->maGripRect.Move( rDelta.X(), rDelta.Y() );
        }
        break;
    }
}
#endif

BOOL OutputDevice::DrawNativeControl( ControlType nType,
                            ControlPart nPart,
                            const Rectangle& rControlRegion,
                            ControlState nState,
                            const ImplControlValue& aValue,
                            ::rtl::OUString aCaption )
{
    if( !lcl_enableNativeWidget( *this ) )
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
    boost::shared_ptr< ImplControlValue > aScreenCtrlValue( lcl_transformControlValue( aValue, *this ) );
    Rectangle screenRegion( ImplLogicToDevicePixel( rControlRegion ) );

    Region aTestRegion( GetActiveClipRegion() );
    aTestRegion.Intersect( rControlRegion );
    if( aTestRegion == rControlRegion )
        nState |= CTRL_CACHING_ALLOWED;   // control is not clipped, caching allowed

    BOOL bRet = mpGraphics->DrawNativeControl(nType, nPart, screenRegion, nState, *aScreenCtrlValue, aCaption, this );

    return bRet;
}


// -----------------------------------------------------------------------

BOOL OutputDevice::DrawNativeControlText(ControlType nType,
                            ControlPart nPart,
                            const Rectangle& rControlRegion,
                            ControlState nState,
                            const ImplControlValue& aValue,
                            ::rtl::OUString aCaption )
{
    if( !lcl_enableNativeWidget( *this ) )
        return FALSE;

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
    boost::shared_ptr< ImplControlValue > aScreenCtrlValue( lcl_transformControlValue( aValue, *this ) );
    Rectangle screenRegion( ImplLogicToDevicePixel( rControlRegion ) );

    BOOL bRet = mpGraphics->DrawNativeControlText(nType, nPart, screenRegion, nState, *aScreenCtrlValue, aCaption, this );

    return bRet;
}


// -----------------------------------------------------------------------

BOOL OutputDevice::GetNativeControlRegion(  ControlType nType,
                                ControlPart nPart,
                                const Rectangle& rControlRegion,
                                ControlState nState,
                                const ImplControlValue& aValue,
                                ::rtl::OUString aCaption,
                                Rectangle &rNativeBoundingRegion,
                                Rectangle &rNativeContentRegion )
{
    if( !lcl_enableNativeWidget( *this ) )
        return FALSE;

    if ( !mpGraphics )
        if ( !ImplGetGraphics() )
            return FALSE;

    // Convert the coordinates from relative to Window-absolute, so we draw
    // in the correct place in platform code
    boost::shared_ptr< ImplControlValue > aScreenCtrlValue( lcl_transformControlValue( aValue, *this ) );
    Rectangle screenRegion( ImplLogicToDevicePixel( rControlRegion ) );

    BOOL bRet = mpGraphics->GetNativeControlRegion(nType, nPart, screenRegion, nState, *aScreenCtrlValue,
                                aCaption, rNativeBoundingRegion,
                                rNativeContentRegion, this );
    if( bRet )
    {
        // transform back native regions
        rNativeBoundingRegion = ImplDevicePixelToLogic( rNativeBoundingRegion );
        rNativeContentRegion = ImplDevicePixelToLogic( rNativeContentRegion );
    }

    return bRet;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
