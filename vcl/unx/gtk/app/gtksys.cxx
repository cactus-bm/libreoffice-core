/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the Original Code is
 *       Michael Meeks <michael.meeks@novell.com>
 * Portions created by the Initial Developer are Copyright (C) 2010 the
 * Initial Developer. All Rights Reserved.
 *
 * Major Contributor(s):
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */
#include <string.h>
#include <gtk/gtk.h>
#include <unx/gtk/gtkinst.hxx>
#include <unx/gtk/gtksys.hxx>

GtkSalSystem *GtkSalSystem::GetSingleton()
{
    static GtkSalSystem *pSingleton = NULL;
    if (!pSingleton)
        pSingleton = new GtkSalSystem();
    return pSingleton;
}

SalSystem *GtkInstance::CreateSalSystem()
{
    return GtkSalSystem::GetSingleton();
}

GtkSalSystem::GtkSalSystem() : UnxSalSystem()
{
    mpDisplay = gdk_display_get_default();
}

GtkSalSystem::~GtkSalSystem()
{
}

GdkScreen *
GtkSalSystem::getScreenMonitorFromIdx (GdkDisplay *pDisplay, int nIdx, gint &nMonitor)
{
    GdkScreen *pScreen = NULL;
    for (gint i = 0; i < gdk_display_get_n_screens (pDisplay); i++)
    {
        pScreen = gdk_display_get_screen (pDisplay, i++);
        if (!pScreen)
            break;
        if (nIdx > gdk_screen_get_n_monitors (pScreen))
            nIdx -= gdk_screen_get_n_monitors (pScreen);
        else
            break;
    }
    nMonitor = nIdx;
    return pScreen;
}

int
GtkSalSystem::getScreenIdxFromPtr (GdkDisplay *pDisplay, GdkScreen *pScreen)
{
    int nIdx = 0;
    for (gint i = 0; i < gdk_display_get_n_screens (pDisplay); i++)
    {
        GdkScreen *pCmp = gdk_display_get_screen (pDisplay, i);
        if (pCmp == pScreen)
            return nIdx;
        nIdx += gdk_screen_get_n_monitors (pCmp);
    }
    g_warning ("failed to find screen %p", pScreen);
    return 0;
}

int GtkSalSystem::getScreenMonitorIdx (GdkDisplay *pDisplay,
                                       GdkScreen *pScreen,
                                       int nX, int nY)
{
    return getScreenIdxFromPtr (pDisplay, pScreen) +
        gdk_screen_get_monitor_at_point (pScreen, nX, nY);
}

unsigned int GtkSalSystem::GetDisplayScreenCount()
{
    gint nMonitor;
    (void)getScreenMonitorFromIdx (mpDisplay, G_MAXINT, nMonitor);
    return G_MAXINT - nMonitor;
}

bool GtkSalSystem::IsMultiDisplay()
{
    return gdk_display_get_n_screens (mpDisplay) > 1;
}

namespace {
int _fallback_get_primary_monitor (GdkScreen *pScreen)
{
    // Use monitor name as primacy heuristic
    int ret = -1;
    int max = gdk_screen_get_n_monitors (pScreen);
    for (int i = 0; i < max && ret < 0; i++)
    {
        char *name = gdk_screen_get_monitor_plug_name (pScreen, i);
        if (!g_ascii_strncasecmp (name, "LVDS", 4))
            ret = i;
        g_free (name);
    }
    return 0;
}

int _get_primary_monitor (GdkScreen *pScreen)
{
    static int (*get_fn) (GdkScreen *) = NULL;
#if GTK_CHECK_VERSION(3,0,0)
    get_fn = gdk_screen_get_primary_monitor;
#endif
    // Perhaps we have a newer gtk+ with this symbol:
    if (!get_fn)
    {
        GModule *module = g_module_open (NULL, (GModuleFlags) 0);
        if (!g_module_symbol (module, "gdk_screen_get_primary_monitor",
                              (gpointer *)&get_fn))
            get_fn = NULL;
        g_module_close (module);
    }
    if (!get_fn)
        get_fn = _fallback_get_primary_monitor;

    return get_fn (pScreen);
}
} // end anonymous namespace

unsigned int GtkSalSystem::GetDefaultDisplayNumber()
{
    GdkScreen *pDefault = gdk_display_get_default_screen (mpDisplay);
    int idx = getScreenIdxFromPtr (mpDisplay, pDefault);
    return idx + _get_primary_monitor (pDefault);
}

Rectangle GtkSalSystem::GetDisplayScreenPosSizePixel (unsigned int nScreen)
{
    gint nMonitor;
    GdkScreen *pScreen;
    GdkRectangle aRect;
    pScreen = getScreenMonitorFromIdx (mpDisplay, nScreen, nMonitor);
    if (!pScreen)
        return Rectangle();
    gdk_screen_get_monitor_geometry (pScreen, nMonitor, &aRect);
    return Rectangle (aRect.x, aRect.y, aRect.width, aRect.height);
}

Rectangle GtkSalSystem::GetDisplayWorkAreaPosSizePixel (unsigned int nScreen)
{
    // FIXME: in theory we need extra code here to collect
    // the work area, ignoring fixed panels etc. on the screen.
    // surely gtk+ should have API to get this for us (?)
    return GetDisplayScreenPosSizePixel( nScreen );
}

rtl::OUString GtkSalSystem::GetScreenName(unsigned int nScreen)
{
    gchar *pStr;
    gint nMonitor;
    GdkScreen *pScreen;
    pScreen = getScreenMonitorFromIdx (mpDisplay, nScreen, nMonitor);
    if (!pScreen)
        return rtl::OUString();
    pStr = gdk_screen_get_monitor_plug_name (pScreen, nMonitor);
    rtl::OUString aRet (pStr, strlen (pStr), RTL_TEXTENCODING_UTF8);
    g_free (pStr);
    return aRet;
}

// convert ~ to indicate mnemonic to '_'
static rtl::OString MapToGtkAccelerator(const String &rStr)
{
    String aRet( rStr );
    aRet.SearchAndReplaceAscii("~", String::CreateFromAscii("_"));
    return rtl::OUStringToOString(aRet, RTL_TEXTENCODING_UTF8);
}

int GtkSalSystem::ShowNativeDialog (const String& rTitle, const String& rMessage,
                                    const std::list< String >& rButtonNames,
                                    int nDefaultButton)
{
    rtl::OString aTitle (rtl::OUStringToOString (rTitle, RTL_TEXTENCODING_UTF8));
    rtl::OString aMessage (rtl::OUStringToOString (rMessage, RTL_TEXTENCODING_UTF8));

    GtkDialog *pDialog = GTK_DIALOG (
        g_object_new (GTK_TYPE_MESSAGE_DIALOG,
                      "title", aTitle.getStr(),
                      "message-type", (int)GTK_MESSAGE_WARNING,
                      "text", aMessage.getStr(),
                      NULL));
    int nButton = 0;
    std::list< String >::const_iterator it;
    for (it = rButtonNames.begin(); it != rButtonNames.end(); ++it)
        gtk_dialog_add_button (pDialog, MapToGtkAccelerator(*it).getStr(), nButton++);
    gtk_dialog_set_default_response (pDialog, nDefaultButton);

    nButton = gtk_dialog_run (pDialog);
    if (nButton < 0)
        nButton = -1;

    gtk_widget_destroy (GTK_WIDGET (pDialog));

    return nButton;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
