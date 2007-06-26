/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: tbxctl.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-26 16:52:41 $
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
#ifndef _BASIDE_TBXCTL_HXX
#define _BASIDE_TBXCTL_HXX

#ifndef _SFXTBXCTRL_HXX //autogen
#include <sfx2/tbxctrl.hxx>
#endif

#ifndef _COM_SUN_STAR_FRAME_XLAYOUTMANAGER_HPP_
#include <com/sun/star/frame/XLayoutManager.hpp>
#endif

/*
#ifdef _BASIDE_POPUPWINDOWTBX

// class PopupWindowTbx --------------------------------------------------

class PopupWindowTbx : public SfxPopupWindow
{
private:
    SfxToolBoxManager   aTbx;
    Link                aSelectLink;

    DECL_LINK( SelectHdl, void* );

public:
    PopupWindowTbx( USHORT nId, WindowAlign eAlign,
                    ResId aRIdWin, ResId aRIdTbx, SfxBindings& rBind );
    ~PopupWindowTbx();

    void                    StartSelection()
                                { aTbx.GetToolBox().StartSelection(); }
    void                    Update();

    virtual SfxPopupWindow* Clone() const;
    virtual void            PopupModeEnd();
};
#endif
*/
//-------------------
// class TbxControls
//-------------------
class TbxControls : public SfxToolBoxControl
{
private:

    struct StateChangedInfo
    {
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XLayoutManager > xLayoutManager;
        bool bDisabled;
    };

    USHORT                  nLastSlot;

protected:
    virtual void            StateChanged( USHORT nSID, SfxItemState eState,
                                          const SfxPoolItem* pState );
public:
    SFX_DECL_TOOLBOX_CONTROL();

    TbxControls(USHORT nSlotId, USHORT nId, ToolBox& rTbx );
    ~TbxControls() {}

    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual SfxPopupWindow*     CreatePopupWindow();

    using                       SfxToolBoxControl::Select;
    void                        Select( USHORT nModifier );
};


#endif // _BASIDE_TBXCTL_HXX

