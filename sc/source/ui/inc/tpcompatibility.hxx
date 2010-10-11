/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyrigt 2010 Novell, Inc.
 *
 * LibreOffice - a multi-platform office productivity suite
 *
 * This file is part of LibreOffice.
 *
 * LibreOffice is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * LibreOffice is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with LibreOffice.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef __SC_TPCOMPATIBILITY_HXX__
#define __SC_TPCOMPATIBILITY_HXX__

#include <sfx2/tabdlg.hxx>
#include <vcl/fixed.hxx>
#include <vcl/lstbox.hxx>

class ScTpCompatOptions : public SfxTabPage
{
public:
    static  SfxTabPage* Create (Window* pParent, const SfxItemSet& rCoreAttrs);

    virtual BOOL FillItemSet(SfxItemSet& rCoreAttrs);
    virtual void Reset(const SfxItemSet& rCoreAttrs);
    virtual int DeactivatePage(SfxItemSet* pSet = NULL);

private:
    explicit ScTpCompatOptions(Window* pParent, const SfxItemSet& rCoreAttrs);
    virtual ~ScTpCompatOptions();

private:
    FixedLine maFlKeyBindings;
    FixedText maFtKeyBindings;
    ListBox   maLbKeyBindings;

};

#endif
