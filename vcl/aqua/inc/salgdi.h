/*************************************************************************
 *
 *  $RCSfile: salgdi.h,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: bmahbod $ $Date: 2000-12-08 02:29:03 $
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

#ifndef _SV_SALGDI_H
#define _SV_SALGDI_H

#include <premac.h>
#include <QD/QD.h>
#include <postmac.h>

#ifndef _SV_SV_H
#include <sv.h>
#endif

#ifndef _SV_VCLWINDOW_H
#include <VCLWindow.h>
#endif

// -------------------
// - SalGraphicsData -
// -------------------

struct SalGraphicsData
{
    // NSView and NSWindow

    VCLVIEW         mhDC;           // VCLVIEW
    VCLWINDOW       mhWnd;          // Window-Handle, when Window-Graphics

    // QuickDraw graph port

    CGrafPtr        mpCGrafPort;        // QD color graphics port

    // Regions within a current port

    RgnHandle       mhClipRgn;      // Clip Region Handle
        RgnHandle       mhGrowRgn;      // Grow Region Handle
    RgnHandle       mhVisiRgn;      // Visible Region Handle

    // Font attributes

    int         mnFontID;       // Mac FontFamilyId
    int         mnFontSize;     // Mac Font Size
        RGBColor                maFontColor;        // Text Color
    Style           mnFontStyle;        // Mac Font Style

    // Pen attributes and status

    BOOL            mbTransparentPen;   // Is pen transparent?
    int         mnPenMode;      // Pen Mode
        RGBColor                maPenColor;     // Pen Color

    // Brush attributes and status

    BOOL            mbTransparentBrush; // Is brush transparent?
        RGBColor                maBrushColor;       // Brush Color

    // Miscellaneous status flags

    BOOL            mbPrinter;      // Is a printer available?
    BOOL            mbVirDev;       // Is a virtual device available?
    BOOL            mbWindow;       // Is a window availble?
    BOOL            mbScreen;       // Is this screen compatiable?
    OSErr           mnMacOSErr;     // The current MacOS error
};

typedef struct SalGraphicsData   SalGraphicsData;
typedef SalGraphicsData         *SalGraphicsDataPtr;

#endif // _SV_SALGDI_H
