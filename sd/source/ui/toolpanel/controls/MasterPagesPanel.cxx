/*************************************************************************
 *
 *  $RCSfile: MasterPagesPanel.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-01-31 14:54:31 $
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

#include "MasterPagesPanel.hxx"
#include "../ScrollPanel.hxx"
#include "CurrentMasterPagesSelector.hxx"
#include "RecentMasterPagesSelector.hxx"
#include "AllMasterPagesSelector.hxx"

#include "DrawViewShell.hxx"
#include "ViewShellBase.hxx"

#include "strings.hrc"
#include "sdresid.hxx"
#include "helpids.h"


namespace sd { namespace toolpanel { namespace controls {


MasterPagesPanel::MasterPagesPanel (TreeNode* pParent, ViewShellBase& rBase)
    : SubToolPanel (pParent)
{
    SdDrawDocument* pDocument = rBase.GetDocument();

    ScrollPanel* pScrollPanel = new ScrollPanel (this);

    // Create a panel with the master pages that are in use by the currently
    // edited document.
    DrawViewShell* pDrawViewShell = static_cast<DrawViewShell*>(
        rBase.GetMainViewShell());
    controls::MasterPagesSelector* pSelector
        = new controls::CurrentMasterPagesSelector (
            this,
            *pDocument,
            rBase,
            *pDrawViewShell);
    pSelector->LateInit();
    pScrollPanel->AddControl (
        ::std::auto_ptr<TreeNode>(pSelector),
        SdResId(STR_TASKPANEL_CURRENT_MASTER_PAGES_TITLE),
        HID_SD_CURRENT_MASTERS);
    pSelector->SetSmartHelpId( SmartId(HID_SD_TASK_PANE_PREVIEW_CURRENT) );

    // Create a panel with the most recently used master pages.
    pSelector = new controls::RecentMasterPagesSelector (
        this,
        *pDocument,
        rBase);
    pSelector->LateInit();
    pScrollPanel->AddControl (
        ::std::auto_ptr<TreeNode>(pSelector),
        SdResId(STR_TASKPANEL_RECENT_MASTER_PAGES_TITLE),
        HID_SD_RECENT_MASTERS);
    pSelector->SetSmartHelpId( SmartId(HID_SD_TASK_PANE_PREVIEW_RECENT) );

    // Create a panel with all available master pages.
    pSelector = new controls::AllMasterPagesSelector (
        this,
        *pDocument,
        rBase,
        *pDrawViewShell);
    pSelector->LateInit();
    pScrollPanel->AddControl (
        ::std::auto_ptr<TreeNode>(pSelector),
        SdResId(STR_TASKPANEL_ALL_MASTER_PAGES_TITLE),
        HID_SD_ALL_MASTERS);

    AddControl (::std::auto_ptr<TreeNode>(pScrollPanel));
    pSelector->SetSmartHelpId( SmartId(HID_SD_TASK_PANE_PREVIEW_ALL) );
}





MasterPagesPanel::~MasterPagesPanel (void)
{
}

} } } // end of namespace ::sd::toolpanel::controls
