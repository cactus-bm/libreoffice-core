/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Copyright 2012 LibreOffice contributors.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef TEMPLATEVIEWITEM_HXX
#define TEMPLATEVIEWITEM_HXX

#include <sfx2/thumbnailviewitem.hxx>

class TemplateViewItem : public ThumbnailViewItem
{
public:

    TemplateViewItem (ThumbnailView &rView, Window *pParent);

    virtual ~TemplateViewItem ();

    void setAuthor (const rtl::OUString &rAuthor) { maAuthor = rAuthor; }

    const rtl::OUString& getAuthor () const { return maAuthor; }

    void setKeywords (const rtl::OUString &rKeywords) { maKeywords = rKeywords; }

    const rtl::OUString& getKeywords () const { return maKeywords; }

    void setFileType (const rtl::OUString &rType) { maFileType = rType; }

    const rtl::OUString& getFileType () const { return maFileType; }

private:

    rtl::OUString maAuthor;
    rtl::OUString maKeywords;
    rtl::OUString maFileType;
};

#endif // TEMPLATEVIEWITEM_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
