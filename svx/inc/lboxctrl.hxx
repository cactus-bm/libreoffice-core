/*************************************************************************
 *
 *  $RCSfile: lboxctrl.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: tl $ $Date: 2001-04-25 13:41:45 $
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

#ifndef _SVX_LBOXCTRL_HXX_
#define _SVX_LBOXCTRL_HXX_

#ifndef _SFXTBXCTRL_HXX
#include <sfx2/tbxctrl.hxx>
#endif

class ToolBox;
class SvxPopupWindowListBox;

/////////////////////////////////////////////////////////////////

class SvxListBoxControl : public SfxToolBoxControl
{
protected:
    String                  aActionStr;
    SvxPopupWindowListBox * pPopupWin;
    USHORT                  nItemId;

    void    Impl_SetInfo( USHORT nCount );

    DECL_LINK( PopupModeEndHdl, void * );
    DECL_LINK( SelectHdl, void * );

public:
    SFX_DECL_TOOLBOX_CONTROL();

    SvxListBoxControl( USHORT nId, ToolBox& rTbx, SfxBindings& rBind );
    virtual ~SvxListBoxControl();

    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual SfxPopupWindow*     CreatePopupWindow();
    virtual void                StateChanged( USHORT nSID,
                                              SfxItemState eState,
                                              const SfxPoolItem* pState );
};

/////////////////////////////////////////////////////////////////

class SvxUndoControl : public SvxListBoxControl
{
public:
    SFX_DECL_TOOLBOX_CONTROL();

    SvxUndoControl( USHORT nId, ToolBox& rTbx, SfxBindings& rBind );
    virtual ~SvxUndoControl();

    virtual SfxPopupWindow*     CreatePopupWindow();
    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual void                StateChanged( USHORT nSID,
                                              SfxItemState eState,
                                              const SfxPoolItem* pState );
};

/////////////////////////////////////////////////////////////////

class SvxRedoControl : public SvxListBoxControl
{
public:
    SFX_DECL_TOOLBOX_CONTROL();

    SvxRedoControl( USHORT nId, ToolBox& rTbx, SfxBindings& rBind );
    virtual ~SvxRedoControl();

    virtual SfxPopupWindow*     CreatePopupWindow();
    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual void                StateChanged( USHORT nSID,
                                              SfxItemState eState,
                                              const SfxPoolItem* pState );
};

/////////////////////////////////////////////////////////////////

#endif

