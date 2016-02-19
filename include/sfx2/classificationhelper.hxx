/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_SFX2_CLASSIFICATIONHELPER_HXX
#define INCLUDED_SFX2_CLASSIFICATIONHELPER_HXX

#include <memory>

#include <rtl/ustring.hxx>
#include <sfx2/dllapi.h>

class SfxObjectShell;
namespace basegfx
{
class BColor;
}

/// Shared code to handle Business Authorization Identification and Labeling Scheme (BAILS) properties.
class SFX2_DLLPUBLIC SfxClassificationHelper
{
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;

public:
    /// Does the document have any BAILS properties?
    static bool IsClassified(SfxObjectShell& rObjectShell);

    SfxClassificationHelper(SfxObjectShell& rObjectShell);
    ~SfxClassificationHelper();
    OUString GetBACName();
    /// If GetImpactLevelColor() will return something meaningful.
    bool HasImpactLevel();
    basegfx::BColor GetImpactLevelColor();
    OUString GetDocumentWatermark();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
