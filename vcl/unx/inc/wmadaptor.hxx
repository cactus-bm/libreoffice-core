/*************************************************************************
 *
 *  $RCSfile: wmadaptor.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: pl $ $Date: 2001-09-10 17:54:44 $
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
 *  Copyright: 2001 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _VCL_WMADAPTOR_HXX_
#define _VCL_WMADAPTOR_HXX_

#ifndef _TL_STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif
#ifndef _PREX_H
#include <prex.h>
#include <X11/Xlib.h>
#include <postx.h>
#endif
#include <vector>

class SalDisplay;
class SalFrame;

namespace vcl_sal {

class WMAdaptor
{
public:
    enum WMAtom {
        // atoms for types
        UTF8_STRING,

        // atoms for extended WM hints
        NET_SUPPORTED,
        NET_SUPPORTING_WM_CHECK,
        NET_WM_NAME,
        NET_WM_ICON_NAME,
        NET_WM_STATE,
        NET_WM_STATE_MAXIMIZED_HORZ,
        NET_WM_STATE_MAXIMIZED_VERT,
        NET_WM_STATE_MODAL,
        NET_WM_STATE_SHADED,
        NET_WM_STATE_SKIP_PAGER,
        NET_WM_STATE_SKIP_TASKBAR,
        NET_WM_STATE_STAYS_ON_TOP,
        NET_WM_STATE_STICKY,
        NET_WM_WINDOW_TYPE,
        NET_WM_WINDOW_TYPE_DESKTOP,
        NET_WM_WINDOW_TYPE_DIALOG,
        NET_WM_WINDOW_TYPE_DOCK,
        NET_WM_WINDOW_TYPE_MENU,
        NET_WM_WINDOW_TYPE_NORMAL,
        NET_WM_WINDOW_TYPE_TOOLBAR,
        NET_NUMBER_OF_DESKTOPS,
        NET_CURRENT_DESKTOP,
        NET_WORKAREA,

        // atoms for Gnome WM hints
        WIN_SUPPORTING_WM_CHECK,
        WIN_PROTOCOLS,
        WIN_WORKSPACE_COUNT,
        WIN_WORKSPACE,
        WIN_LAYER,
        WIN_STATE,
        WIN_HINTS,
        WIN_APP_STATE,
        WIN_EXPANDED_SIZE,
        WIN_ICONS,
        WIN_WORKSPACE_NAMES,
        WIN_CLIENT_LIST,

        // atoms for general WM hints
        WM_STATE,
        MOTIF_WM_HINTS,
        WM_PROTOCOLS,
        WM_DELETE_WINDOW,
        WM_SAVE_YOURSELF,
        WM_COMMAND,

        // special atoms
        SAL_QUITEVENT,
        SAL_USEREVENT,
        SAL_EXTTEXTEVENT,
        DTWM_IS_RUNNING,
        NetAtomMax
    };

    /*
     *  flags for frame decoration
     */
    static const int decoration_Title           = 0x00000001;
    static const int decoration_Border      = 0x00000002;
    static const int decoration_Resize      = 0x00000004;
    static const int decoration_MinimizeBtn = 0x00000008;
    static const int decoration_MaximizeBtn = 0x00000010;
    static const int decoration_CloseBtn        = 0x00000020;
    static const int decoration_All         = 0x10000000;

    /*
     *  window type
     */
    enum WMWindowType
    {
        windowType_Normal,
        windowType_ModalDialogue,
        windowType_ModelessDialogue,
        windowType_OverrideRedirect
    };

protected:
    SalDisplay*             m_pSalDisplay;      // Display to use
    Display*                m_pDisplay;         // X Display of SalDisplay
    String                  m_aWMName;
    Atom                    m_aWMAtoms[ NetAtomMax];
    int                     m_nDesktops;
    bool                    m_bEqualWorkAreas;
    ::std::vector< Rectangle >
                            m_aWMWorkAreas;

    WMAdaptor( SalDisplay * )
;
    void initAtoms();
    bool getNetWmName();

    /*
     *  returns whether this instance is useful
     *  only useful for createWMAdaptor
     */
    virtual bool isValid() const;

public:
    virtual ~WMAdaptor();

    /*
     *  creates a vaild WMAdaptor instance for the SalDisplay
     */
    static WMAdaptor* createWMAdaptor( SalDisplay* );

    /*
     *  may return an empty string if the window manager could
     *  not be identified.
     */
    const String& getWindowManagerName() const
    { return m_aWMName; }

    /*
     *  sets window title
     */
    virtual void setWMName( SalFrame* pFrame, const String& rWMName ) const;

    /*
     *  maximizes frame
     *  maximization can be toggled in either direction
     *  to get the original position and size
     *  use maximizeFrame( pFrame, false, false )
     */
    virtual void maximizeFrame( SalFrame* pFrame, bool bHorizontal = true, bool bVertical = true ) const;

    /*
     *  set hints what decoration is needed;
     *  must be called before showing the frame
     */
    virtual void setFrameTypeAndDecoration( SalFrame* pFrame, WMWindowType eType, int nDecorationFlags, SalFrame* pTransientFrame = NULL ) const;

    /*
     *  enables always on top or equivalent if possible
     */
    virtual void enableAlwaysOnTop( SalFrame* pFrame, bool bEnable ) const;

    /*
     *  gets a WM atom
     */
    Atom getAtom( WMAtom eAtom ) const
    { return m_aWMAtoms[ eAtom ]; }

    /*
     * supports correct positioning
     */

    virtual bool supportsICCCMPos () const;

    int getPositionWinGravity () const
    {
        if (m_aWMName.EqualsAscii ("Dtwm"))
            return CenterGravity;
        return StaticGravity;
    }
};

} // namespace

#endif
