/*************************************************************************
 *
 *  $RCSfile: CurrentMasterPagesSelector.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2005-02-16 16:59:06 $
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

#ifndef SD_TOOLPANEL_CONTROLS_CURRENT_MASTER_PAGES_SELECTOR_HXX
#define SD_TOOLPANEL_CONTROLS_CURRENT_MASTER_PAGES_SELECTOR_HXX

#include "MasterPagesSelector.hxx"
#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif

namespace sd { namespace toolpanel { namespace controls {


/** Adaption of a value set control that shows previews of master pages that
    are currently used by the document of the application.

    It does not use the approach of the base class of storing tokens as item
    data in the value set that identify entries in the MasterPageContainer.
    Instead it stores pointers directly to master pages in the document.
    The base class is used to have access to the code that assigns master
    pages to the document.
*/
class CurrentMasterPagesSelector
    : public MasterPagesSelector
{
public:
    CurrentMasterPagesSelector (
        TreeNode* pParent,
        SdDrawDocument& rDocument,
        ViewShellBase& rBase,
        DrawViewShell& rViewShell);
    virtual ~CurrentMasterPagesSelector (void);

    virtual void LateInit (void);

    /** Set the selection so that the master page is selected that is
        used by the currently selected page of the document in the
        center pane.
    */
    virtual void UpdateSelection (void);

    /** Clear the list of currently used master pages and fill it
        with anew.
    */
    virtual void Fill (void);
    virtual void Clear (void);

    virtual void InvalidatePreview (const SdPage* pPage);

protected:
    /** Return the master page whose preview is currently selected in the
        value set control.
        @return
            The returned page belongs to the main document, not to the local
            document of the MasterPageContainer.
    */
    virtual SdPage* GetSelectedMasterPage (void);

    /** For all currently used master pages a new preview is rendered and
        put into the value set control.
    */
    virtual void UpdateAllPreviews (void);
    virtual void UpdatePreview (USHORT nIndex);

private:
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent>
        mxListener;
};

} } } // end of namespace ::sd::toolpanel::controls

#endif
