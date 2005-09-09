/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ViewShellImplementation.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 05:20:06 $
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

#ifndef SD_VIEW_SHELL_IMPLEMENTATION_HXX
#define SD_VIEW_SHELL_IMPLEMENTATION_HXX

#include "ViewShell.hxx"

namespace sd {

/** This class contains (will contain) the implementation of methods that
    have not be accessible from the outside.
*/
class ViewShell::Implementation
{
public:
    bool mbIsShowingUIControls;
    bool mbIsMainViewShell;
    /// Set to true when the ViewShell::Init() method has been called.
    bool mbIsInitialized;

    Implementation (ViewShell& rViewShell);
    ~Implementation (void);

    /** Process the SID_MODIFY slot.
    */
    void ProcessModifyPageSlot (
        SfxRequest& rRequest,
        SdPage* pCurrentPage,
        PageKind ePageKind);

    /** Assign the given layout to the given page.  This method is at the
        moment merely a front end for ProcessModifyPageSlot.
        @param pPage
            If a NULL pointer is given then this call is ignored.
    */
    void AssignLayout (
        SdPage* pPage,
        AutoLayout aLayout);

    /** Determine the view id of the view shell.  This corresponds to the
        view id stored in the SfxViewFrame class.

        We can not use the view of that class because with the introduction
        of the multi pane GUI we do not switch the SfxViewShell anymore when
        switching the view in the center pane.  The view id of the
        SfxViewFrame is thus not modified and we can not set it from the
        outside.

        The view id is still needed for the SFX to determine on start up
        (e.g. after loading a document) which ViewShellBase sub class to
        use.  These sub classes--like OutlineViewShellBase--exist only to be
        used by the SFX as factories.  They only set the initial pane
        configuration, nothing more.

        So what we do here in essence is to return on of the
        ViewShellFactoryIds that can be used to select the factory that
        creates the ViewShellBase subclass with the initial pane
        configuration that has in the center pane a view shell of the same
        type as mrViewShell.
    */
    sal_uInt16 GetViewId (void);

private:
    ViewShell& mrViewShell;
};


} // end of namespace sd

#endif
