/*************************************************************************
 *
 *  $RCSfile: prex.h,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: vg $ $Date: 2004-01-06 14:23:45 $
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

#ifndef _PREX_H
#define _PREX_H

#define Window      XLIB_Window
#define BYTE        XLIB_BYTE
#define INT8        XLIB_INT8
#define BOOL        XLIB_BOOL
#define Font        XLIB_Font
#define Cursor      XLIB_Cursor
#define String      XLIB_String
#define KeyCode     XLIB_KeyCode
#define Region      XLIB_Region
#define Icon        XLIB_Icon
#define class       XLIB_class
#define new         XLIB_new
#define Time        XLIB_Time
#define Region      XLIB_Region
#define Boolean     XLIB_Boolean

/* fuer Network Audio System */
#define Sound       XLIB_Sound

#if defined __cplusplus
extern "C" {
#endif
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/StringDefs.h>
typedef unsigned long Pixel;

#undef  DestroyAll
#define DestroyAll      XLIB_DestroyAll
#define XLIB_DestroyAll 0
#undef  String
#define String          XLIB_String

#undef  KeyCode
#define KeyCode         XLIB_KeyCode //undef in intrinsics

#define __Ol_OlXlibExt_h__

#if defined(_USE_PRINT_EXTENSION_)
      #include <X11/X.h>
      #include <X11/Xlib.h>
      #include <X11/Xutil.h>

      Bool XSalIsDisplay( Display *p_display );
      Bool XSalIsPrinter( Display *p_display );
      Display* GetXpDisplay();
#endif

#endif

