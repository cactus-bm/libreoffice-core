/*************************************************************************
 *
 *  $RCSfile: salvd.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: kz $ $Date: 2003-11-18 14:46:37 $
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

#define _SV_SALVD_CXX

// -=-= #includes =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include <salunx.h>

#ifndef _SV_SALDATA_HXX
#include <saldata.hxx>
#endif
#ifndef _SV_SALDISP_HXX
#include <saldisp.hxx>
#endif
#ifndef _SV_SALINST_HXX
#include <salinst.hxx>
#endif
#ifndef _SV_SALGDI_H
#include <salgdi.h>
#endif
#ifndef _SV_SALVD_H
#include <salvd.h>
#endif

// -=-= SalInstance =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
SalVirtualDevice* X11SalInstance::CreateVirtualDevice( SalGraphics* pGraphics,
                                                       long nDX, long nDY,
                                                       USHORT nBitCount )
{
    X11SalVirtualDevice *pVDev = new X11SalVirtualDevice();
    if( !nBitCount && pGraphics )
        nBitCount = pGraphics->GetBitCount();
    if( !pVDev->Init( GetSalData()->GetDefDisp(), nDX, nDY, nBitCount ) )
    {
        delete pVDev;
        return NULL;
    }

    pVDev->InitGraphics( pVDev );
    return pVDev;
}

void X11SalInstance::DestroyVirtualDevice( SalVirtualDevice* pDevice )
{
    delete pDevice;
}

// -=-= SalGraphicsData =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::Init( X11SalVirtualDevice *pDevice )
{
    SalDisplay *pDisplay  = pDevice->GetDisplay();

    int nVisualDepth = pDisplay->GetColormap().GetVisual()->GetDepth();
    int nDeviceDepth = pDevice->GetDepth();

    if( nDeviceDepth == nVisualDepth )
        xColormap_ = &pDisplay->GetColormap();
    else
    if( nDeviceDepth == 1 )
        xColormap_ = new SalColormap();

    hDrawable_   = pDevice->GetDrawable();
    m_pVDev      = pDevice;
    m_pFrame     = NULL;

    bWindow_     = pDisplay->IsDisplay();
    bVirDev_     = TRUE;

    nPenPixel_   = GetPixel( nPenColor_ );
    nTextPixel_  = GetPixel( nTextColor_ );
    nBrushPixel_ = GetPixel( nBrushColor_ );
}

// -=-= SalVirDevData / SalVirtualDevice -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
BOOL X11SalVirtualDevice::Init( SalDisplay *pDisplay,
                                long nDX, long nDY,
                                USHORT nBitCount )
{
    pDisplay_               = pDisplay;
    pGraphics_              = new X11SalGraphics();
    pGraphics_->SetLayout( 0 ); // by default no! mirroring for VirtualDevices, can be enabled with EnableRTL()
    nDX_                    = nDX;
    nDY_                    = nDY;
    nDepth_                 = nBitCount;

    hDrawable_              = XCreatePixmap( GetXDisplay(),
                                     pDisplay_->GetDrawable(),
                                     nDX_, nDY_,
                                     GetDepth() );

    pGraphics_->Init( this );

    return hDrawable_ != None ? TRUE : FALSE;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
X11SalVirtualDevice::X11SalVirtualDevice()
{
    pDisplay_               = (SalDisplay*)ILLEGAL_POINTER;
    pGraphics_              = NULL;
    hDrawable_              = None;
    nDX_                    = 0;
    nDY_                    = 0;
    nDepth_                 = 0;
    bGraphics_              = FALSE;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
X11SalVirtualDevice::~X11SalVirtualDevice()
{
    if( pGraphics_ )
        delete pGraphics_;

    if( GetDrawable() )
        XFreePixmap( GetXDisplay(), GetDrawable() );
}

SalGraphics* X11SalVirtualDevice::GetGraphics()
{
    if( bGraphics_ )
        return NULL;

    if( pGraphics_ )
        bGraphics_ = TRUE;

    return pGraphics_;
}

void X11SalVirtualDevice::ReleaseGraphics( SalGraphics* )
{ bGraphics_ = FALSE; }

BOOL X11SalVirtualDevice::SetSize( long nDX, long nDY )
{
    if( !nDX ) nDX = 1;
    if( !nDY ) nDY = 1;

    Pixmap h = XCreatePixmap( GetXDisplay(),
                              pDisplay_->GetDrawable(),
                              nDX, nDY, nDepth_ );

    if( !h )
    {
        if( !GetDrawable() )
        {
            hDrawable_ = XCreatePixmap( GetXDisplay(),
                                        pDisplay_->GetDrawable(),
                                        1, 1, nDepth_ );
            nDX_ = 1;
            nDY_ = 1;
        }
        return FALSE;
    }

    if( GetDrawable() )
        XFreePixmap( GetXDisplay(), GetDrawable() );
    hDrawable_ = h;

    nDX_ = nDX;
    nDY_ = nDY;

    if( pGraphics_ )
        InitGraphics( this );

    return TRUE;
}

