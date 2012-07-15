/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __SFX2_TEMPLATEABSTRACTVIEW_HXX__
#define __SFX2_TEMPLATEABSTRACTVIEW_HXX__

#include <sfx2/thumbnailview.hxx>

class TemplateView;

class SFX2_DLLPUBLIC TemplateAbstractView : public ThumbnailView
{
public:

    TemplateAbstractView (Window *pParent, WinBits nWinStyle, bool bDisableTransientChildren);

    TemplateAbstractView ( Window* pParent, const ResId& rResId, bool bDisableTransientChildren = false );

    virtual ~TemplateAbstractView ();

    // Fill view with template folders thumbnails
    virtual void Populate () = 0;

    virtual void showOverlay (bool bVisible) = 0;

    // Check if the overlay is visible or not.
    bool isOverlayVisible () const;

    void sortOverlayItems (const boost::function<bool (const ThumbnailViewItem*,
                                                       const ThumbnailViewItem*) > &func);

    void setOverlayDblClickHdl (const Link &rLink);

    void setOverlayCloseHdl (const Link &rLink);

protected:

    virtual void OnSelectionMode (bool bMode);

protected:

    TemplateView *mpItemView;
};

#endif // __SFX2_TEMPLATEABSTRACTVIEW_HXX__

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
