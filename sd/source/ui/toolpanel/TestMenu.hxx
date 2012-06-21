/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#ifndef SD_TASKPANE_COLOR_MENU_HXX
#define SD_TASKPANE_COLOR_MENU_HXX

#include "taskpane/TaskPaneTreeNode.hxx"
#include <svtools/valueset.hxx>
#include <vcl/window.hxx>

#include <memory>


namespace sd { namespace toolpanel {

class ControlFactory;

#ifdef SHOW_COLOR_MENU

/** This demo menu shows the colors that are available from the
    StyleSettings.
*/
class ColorMenu
    : public ::Window,
      public TreeNode
{
public:
    ColorMenu (::Window* i_pParent);
    virtual ~ColorMenu (void);

    static ::std::auto_ptr<ControlFactory> CreateControlFactory (void);

    // From ILayoutableWindow
    virtual Size GetPreferredSize (void);
    virtual sal_Int32 GetPreferredWidth (sal_Int32 nHeight);
    virtual sal_Int32 GetPreferredHeight (sal_Int32 nWidth);
    virtual bool IsResizable (void);
    virtual ::Window* GetWindow (void);

    // From ::Window
    virtual void Resize (void);

    using Window::GetWindow;

private:
    ValueSet maSet;
    int mnPreferredColumnCount;

    /** Depending on the given number of columns and the item size
        calculate the number of rows that are necessary to display all
        items.
    */
    int CalculateRowCount (const Size& rItemSize, int nColumnCount);
    void Fill (void);
};
#endif

} } // end of namespace ::sd::toolpanel

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
