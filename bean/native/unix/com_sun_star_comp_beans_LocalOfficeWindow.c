/*************************************************************************
 *
 *  $RCSfile: com_sun_star_comp_beans_LocalOfficeWindow.c,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2005-03-15 09:07:33 $
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>

#include "jni.h"
#include "jawt.h"
#include "jawt_md.h"

//#include "../inc/com_sun_star_comp_beans_LocalOfficeWindow.h"

#if defined assert
#undef assert
#endif

#define assert(X) if (!X) { (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/RuntimeException"), "assertion failed"); return;}


#define SYSTEM_WIN32   1
#define SYSTEM_WIN16   2
#define SYSTEM_JAVA    3
#define SYSTEM_OS2     4
#define SYSTEM_MAC     5
#define SYSTEM_XWINDOW 6

/*****************************************************************************/
/*
 * Class:     com_sun_star_comp_beans_LocalOfficeWindow
 * Method:    getNativeWindowSystemType
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_sun_star_comp_beans_LocalOfficeWindow_getNativeWindowSystemType
  (JNIEnv * env, jobject obj_this)
{
    return (SYSTEM_XWINDOW);
}


/*****************************************************************************/
/*
 * Class:     com_sun_star_beans_LocalOfficeWindow
 * Method:    getNativeWindow
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_sun_star_comp_beans_LocalOfficeWindow_getNativeWindow
  (JNIEnv * env, jobject obj_this)
{
    jboolean result;
    jint lock;

    JAWT awt;
    JAWT_DrawingSurface* ds;
    JAWT_DrawingSurfaceInfo* dsi;
    JAWT_X11DrawingSurfaceInfo* dsi_x11;

    Drawable drawable;
    Display* display;

    /* Get the AWT */
    awt.version = JAWT_VERSION_1_3;
    result = JAWT_GetAWT(env, &awt);
    assert(result != JNI_FALSE);

                                /* Get the drawing surface */
    if ((ds = awt.GetDrawingSurface(env, obj_this)) == NULL)
        return 0L;

    /* Lock the drawing surface */
    lock = ds->Lock(ds);
    assert((lock & JAWT_LOCK_ERROR) == 0);

    /* Get the drawing surface info */
    dsi = ds->GetDrawingSurfaceInfo(ds);

    /* Get the platform-specific drawing info */
    dsi_x11 = (JAWT_X11DrawingSurfaceInfo*)dsi->platformInfo;

    drawable = dsi_x11->drawable;
    display  = dsi_x11->display;

    /* Free the drawing surface info */
    ds->FreeDrawingSurfaceInfo(dsi);
    /* Unlock the drawing surface */
    ds->Unlock(ds);
    /* Free the drawing surface */
    awt.FreeDrawingSurface(ds);

    return ((jlong)drawable);
}











