/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <sal/types.h>

#define LOK_USE_UNSTABLE_API
#include <LibreOfficeKit/LibreOfficeKit.h>

#include <LibreOfficeKit/LibreOfficeKitGtk.h>

static void lok_docview_class_init( LOKDocViewClass* pClass );
static void lok_docview_init( LOKDocView* pDocView );

// We specifically need to destroy the document when closing in order to ensure
// that lock files etc. are cleaned up.
void lcl_onDestroy( LOKDocView* pDocView, gpointer pData )
{
    (void) pData;
    if ( pDocView->pDocument )
        pDocView->pDocument->pClass->destroy( pDocView->pDocument );
    pDocView->pDocument = NULL;
}

SAL_DLLPUBLIC_EXPORT guint lok_docview_get_type()
{
    static guint lok_docview_type = 0;

    if (!lok_docview_type)
    {
        GtkTypeInfo lok_docview_info =
        {
            "LokDocView",
            sizeof( LOKDocView ),
            sizeof( LOKDocViewClass ),
            (GtkClassInitFunc) lok_docview_class_init,
            (GtkObjectInitFunc) lok_docview_init,
            NULL,
            NULL,
            (GtkClassInitFunc) NULL
        };

        lok_docview_type = gtk_type_unique( gtk_scrolled_window_get_type(), &lok_docview_info );
    }
    return lok_docview_type;
}

static void lok_docview_class_init( LOKDocViewClass* pClass )
{
    pClass->lok_docview = NULL;
}

static void lok_docview_init( LOKDocView* pDocView )
{
    // Gtk ScrolledWindow is apparently not fully initialised yet, we specifically
    // have to set the [hv]adjustment to prevent GTK assertions from firing, see
    // https://bugzilla.gnome.org/show_bug.cgi?id=438114 for more info.
    gtk_scrolled_window_set_hadjustment( GTK_SCROLLED_WINDOW( pDocView ), NULL );
    gtk_scrolled_window_set_vadjustment( GTK_SCROLLED_WINDOW( pDocView ), NULL );

    pDocView->pEventBox = gtk_event_box_new();
    gtk_scrolled_window_add_with_viewport( GTK_SCROLLED_WINDOW(pDocView),
                                           pDocView->pEventBox );

    pDocView->pCanvas = gtk_image_new();
    gtk_container_add( GTK_CONTAINER( pDocView->pEventBox ), pDocView->pCanvas );

    gtk_widget_show( pDocView->pCanvas );
    gtk_widget_show( pDocView->pEventBox );

    pDocView->pPixBuf = 0;

    // TODO: figure out a clever view of getting paths set up.
    pDocView->pOffice = 0;
    pDocView->pDocument = 0;

    pDocView->fZoom = 1;

    gtk_signal_connect( GTK_OBJECT(pDocView), "destroy",
                        GTK_SIGNAL_FUNC(lcl_onDestroy), NULL );
}

SAL_DLLPUBLIC_EXPORT GtkWidget* lok_docview_new( LibreOfficeKit* pOffice )
{
    LOKDocView* pDocView = gtk_type_new( lok_docview_get_type() );
    pDocView->pOffice = pOffice;
    return GTK_WIDGET( pDocView );
}

void renderDocument( LOKDocView* pDocView )
{
    long nWidth, nHeight;
    int nRenderWidth, nRenderHeight;
    unsigned char* pBuffer;
    int nRowStride;

    g_assert( pDocView->pDocument );

    if ( pDocView->pPixBuf )
    {
        g_object_unref( G_OBJECT( pDocView->pPixBuf ) );
    }

    pDocView->pDocument->pClass->getDocumentSize( pDocView->pDocument, &nWidth, &nHeight );

    // Draw the whole document at once (for now)

    // TODO: we really should scale by screen DPI here -- 10 seems to be a vaguely
    // correct factor for my screen at least.
    nRenderWidth = nWidth * pDocView->fZoom / 10;
    nRenderHeight = nHeight * pDocView->fZoom / 10;

    pDocView->pPixBuf = gdk_pixbuf_new( GDK_COLORSPACE_RGB,
                                        TRUE, 8,
                                        nRenderWidth, nRenderHeight);


    pBuffer = gdk_pixbuf_get_pixels( pDocView->pPixBuf );
    pDocView->pDocument->pClass->paintTile( pDocView->pDocument,
                                            pBuffer,
                                            nRenderWidth, nRenderHeight,
                                            &nRowStride,
                                            0, 0, // origin
                                            nWidth, nHeight );
    // TODO: double check that the rowstride really matches what we expected,
    // although presumably we'd already be crashing by now if things were
    // wrong.
    (void) nRowStride;

    gtk_image_set_from_pixbuf( GTK_IMAGE( pDocView->pCanvas ), pDocView->pPixBuf );
}

SAL_DLLPUBLIC_EXPORT gboolean lok_docview_open_document( LOKDocView* pDocView, char* pPath )
{
    if ( pDocView->pDocument )
    {
        pDocView->pDocument->pClass->destroy( pDocView->pDocument );
        pDocView->pDocument = NULL;
    }

    pDocView->pDocument = pDocView->pOffice->pClass->documentLoad( pDocView->pOffice,
                                                                   pPath );
    if ( !pDocView->pDocument )
    {
        // FIXME: should have a GError parameter and populate it.
        char *pError = pDocView->pOffice->pClass->getError( pDocView->pOffice );
        fprintf( stderr, "Error opening document '%s'\n", pError );
        return FALSE;
    }
    else
        renderDocument( pDocView );

    return TRUE;
}

SAL_DLLPUBLIC_EXPORT void lok_docview_set_zoom ( LOKDocView* pDocView, float fZoom )
{
    pDocView->fZoom = fZoom;
    renderDocument( pDocView );
    // TODO: maybe remember and reset positiong?
}

SAL_DLLPUBLIC_EXPORT float lok_docview_get_zoom ( LOKDocView* pDocView )
{
    return pDocView->fZoom;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
