/*************************************************************************
 *
 *  $RCSfile: tbx_ww.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2003-04-01 13:44:31 $
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


#ifndef _SD_TBX_WW_HXX
#define _SD_TBX_WW_HXX

#ifndef _SDRESID_HXX
#include <sdresid.hxx>
#endif

#ifndef _SFXTBXMGR_HXX //autogen
#include <sfx2/tbxmgr.hxx>
#endif

#ifndef _SFXTBXCTRL_HXX //autogen
#include <sfx2/tbxctrl.hxx>
#endif

//------------------------------------------------------------------------

class SdPopupWindowTbx : public SfxPopupWindow
{
private:
    SfxToolBoxManager   aTbx;
    SdResId             aSdResIdWin;
    SdResId             aSdResIdTbx;
    WindowAlign         eTbxAlign;
    Link                aSelectLink;

    DECL_LINK( TbxSelectHdl, ToolBox * );

public:
    SdPopupWindowTbx( USHORT nId, WindowAlign eAlign,
                      SdResId aRIdWin, SdResId aRIdTbx, SfxBindings& rBindings );
    ~SdPopupWindowTbx();

    virtual SfxPopupWindow* Clone() const;

    virtual void PopupModeEnd();

    void         StartSelection() { aTbx.GetToolBox().StartSelection(); }
    void         Update();

private:
    /** Adapt the toolbox to whether CTL is active or not.  If CTL is not
        active then all toolbox items that describe CTL specific shapes are
        deleted from it and its size is re-calculated.
    */
    void AdaptToCTL (void);
};

//------------------------------------------------------------------------

class SdTbxControl : public SfxToolBoxControl
{
private:
    BOOL    IsCheckable( USHORT nSId );

public:
            SFX_DECL_TOOLBOX_CONTROL();

            SdTbxControl(USHORT nId, ToolBox& rTbx, SfxBindings& rBind );
            ~SdTbxControl() {}

    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual SfxPopupWindow*     CreatePopupWindow();
    virtual void StateChanged( USHORT nSId, SfxItemState eState,
                                const SfxPoolItem* pState );
};

#endif      // _SD_TBX_WW_HXX

