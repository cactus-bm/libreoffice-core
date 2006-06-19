/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: i18n_wrp.cxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 19:49:40 $
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

struct XIMArg
{
    char *name;
    char *value;
};

#if defined(SOLARIS) && !defined(__GNUC__)
#include <varargs.h>
#else
#include <stdarg.h>
#endif
#include <sal/alloca.h>

#include <string.h>
#if !defined(MACOSX)
/* MacOS X doesn't yet support XIM... FIXME */
#include <dlfcn.h>
#endif
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include "XIM.h"

#define XIIIMP_LIB       "xiiimp.so.2"

#ifdef SOLARIS
#define XIIIMP_PATH     "/usr/openwin/lib/locale/common/" XIIIMP_LIB
#else /* Linux */
#define XIIIMP_PATH     "/usr/lib/im/" XIIIMP_LIB
#endif

/* global variables */
static void *g_dlmodule = 0;

extern "C" {
typedef XIM (*OpenFunction)(Display*, XrmDatabase, char*, char*, XIMArg*);
}

static OpenFunction g_open_im = (OpenFunction)NULL;

/* utility function to transform vararg list into an array of XIMArg */

int
XvaCountArgs( XIMArg *pInArgs )
{
    int nArgs = 0;
    char *pName, *pValue;

    while ( (pName = pInArgs->name) != NULL )
    {
        pValue = pInArgs->value;

        if ( strcmp(pName, XNVaNestedList) == 0 )
        {
            nArgs += XvaCountArgs( (XIMArg*)pValue );
        }
        else
        {
            nArgs += 1;
        }
        pInArgs++;
    }

    return nArgs;
}

int
XvaCountArgs( va_list pInArgs )
{
    int nArgs = 0;
    char *pName, *pValue;

    while ( (pName = va_arg(pInArgs, char*)) != NULL)
    {
        pValue = va_arg(pInArgs, char*);

        if ( strcmp(pName, XNVaNestedList) == 0 )
        {
            nArgs += XvaCountArgs( (XIMArg*)pValue );
        }
        else
        {
            nArgs += 1;
        }
    }

    return nArgs;
}

XIMArg*
XvaGetArgs( XIMArg *pInArgs, XIMArg *pOutArgs )
{
    char *pName, *pValue;

    while ( (pName = pInArgs->name) != NULL )
    {
        pValue = pInArgs->value;

        if ( strcmp(pName, XNVaNestedList) == 0 )
        {
            pOutArgs = XvaGetArgs( (XIMArg*)pValue, pOutArgs );
        }
        else
        {
            pOutArgs->name  = pName;
            pOutArgs->value = pValue;
            pOutArgs++;
        }
        pInArgs++;
    }

    return pOutArgs;
}

void
XvaGetArgs( va_list pInArgs, XIMArg *pOutArgs )
{
    char *pName, *pValue;

    while ((pName = va_arg(pInArgs, char*)) != NULL)
    {
        pValue = va_arg(pInArgs, char*);

        if ( strcmp(pName, XNVaNestedList) == 0 )
        {
            pOutArgs = XvaGetArgs( (XIMArg*)pValue, pOutArgs );
        }
        else
        {
            pOutArgs->name  = pName;
            pOutArgs->value = pValue;
            pOutArgs++;
        }
    }

    pOutArgs->name  = NULL;
    pOutArgs->value = NULL;
}


/* Puplic functions */

#ifdef __cplusplus
extern "C"
#endif
XIM
XvaOpenIM(Display *display, XrmDatabase rdb,
        char *res_name, char *res_class, ...)
{
      XIM xim = (XIM)0;
      va_list variable;
      int total_count = 0;

      /*
        * so count the stuff dangling here
     */

#if defined(SOLARIS) && !defined(__GNUC__)
      va_start(variable);
#else
    va_start(variable, res_class);
#endif
      total_count = XvaCountArgs(variable);
      va_end(variable);

      if (total_count > 0)
    {
        /* call a new open IM method */

        XIMArg* args = (XIMArg*)alloca( (total_count + 1) * sizeof(XIMArg) );

        /*
          * now package it up so we can set it along
          */
#if defined(SOLARIS) && !defined(__GNUC__)
        va_start(variable);
#else
        va_start(variable, res_class);
#endif
        XvaGetArgs( variable, args );
        va_end(variable);

    /* MacOS X doesn't yet support XIM... FIXME */
#if !defined(MACOSX)
        if (!g_dlmodule)
        {
            g_dlmodule = dlopen(XIIIMP_LIB, RTLD_LAZY);
            if(!g_dlmodule)
            {
                g_dlmodule = dlopen(XIIIMP_PATH, RTLD_LAZY);
                if (!g_dlmodule)
                    goto legacy_XIM;
            }
              g_open_im = (OpenFunction)(long)dlsym(g_dlmodule, "__XOpenIM");
              if (!g_open_im)
                goto legacy_XIM;

              xim = (*g_open_im)(display, (XrmDatabase)rdb,
                  (char*)res_name, (char *)res_class, (XIMArg*)args);
        }
        else
        {
              goto legacy_XIM;
        }
#endif
      }

     legacy_XIM:

    if (!xim)
        xim = XOpenIM(display, rdb, res_name, res_class);

    return xim;
}

/*
 * Close the connection to the input manager, and free the XIM structure
 */

Status XvaCloseIM(XIM)
{
      Status s = False;

    /* MacOS X doesn't yet support XIM... FIXME */
#if !defined(MACOSX)
    if (!g_dlmodule)
    {
        /* assuming one XvaOpenIM call */
        dlclose(g_dlmodule);
            g_dlmodule = (void*)0;
        g_open_im = (OpenFunction)NULL;
        s = True;
      }
#endif
    return (s);
}



