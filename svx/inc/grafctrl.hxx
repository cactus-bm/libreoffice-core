/*************************************************************************
 *
 *  $RCSfile: grafctrl.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: kz $ $Date: 2005-01-21 14:43:09 $
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

#ifndef _SVX_GRAFCTRL_HXX
#define _SVX_GRAFCTRL_HXX

#ifndef _SFXLSTNER_HXX //autogen
#include <svtools/lstner.hxx>
#endif
#ifndef _SFXINTITEM_HXX
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXTBXCTRL_HXX //autogen
#include <sfx2/tbxctrl.hxx>
#endif

#ifndef INCLUDED_SVXDLLAPI_H
#include "svx/svxdllapi.h"
#endif

// ----------------
// - TbxImageItem -
// ----------------

class SVX_DLLPUBLIC TbxImageItem : public SfxUInt16Item
{
public:
                            TYPEINFO();
                            TbxImageItem( USHORT nWhich = 0, UINT16 nImage = 0 );

    virtual SfxPoolItem*    Clone( SfxItemPool* pPool = 0 ) const;
    virtual int             operator==( const SfxPoolItem& ) const;
};

// -------------------------------
// - SvxGrafFilterToolBoxControl -
// -------------------------------

class SVX_DLLPUBLIC SvxGrafFilterToolBoxControl : public SfxToolBoxControl
{
public:

                                SFX_DECL_TOOLBOX_CONTROL();

                                SvxGrafFilterToolBoxControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
                                ~SvxGrafFilterToolBoxControl();

    virtual void                StateChanged( USHORT nSID, SfxItemState eState, const SfxPoolItem* pState );
    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual SfxPopupWindow*     CreatePopupWindow();
};

// -------------------------
// - SvxGrafToolBoxControl -
// -------------------------

class SvxGrafToolBoxControl : public SfxToolBoxControl
{
public:

                        SFX_DECL_TOOLBOX_CONTROL();
                        SvxGrafToolBoxControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
                        ~SvxGrafToolBoxControl();

    virtual void        StateChanged( USHORT nSID, SfxItemState eState, const SfxPoolItem* pState );
    virtual Window*     CreateItemWindow( Window *pParent );
};

// ----------------------------
// - SvxGrafRedToolBoxControl -
// ----------------------------

class SVX_DLLPUBLIC SvxGrafRedToolBoxControl : public SvxGrafToolBoxControl
{
public:
                        SFX_DECL_TOOLBOX_CONTROL();
                        SvxGrafRedToolBoxControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
};

// ------------------------------
// - SvxGrafGreenToolBoxControl -
// ------------------------------

class SVX_DLLPUBLIC SvxGrafGreenToolBoxControl : public SvxGrafToolBoxControl
{
public:
                        SFX_DECL_TOOLBOX_CONTROL();
                        SvxGrafGreenToolBoxControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
};

// -----------------------------
// - SvxGrafBlueToolBoxControl -
// -----------------------------

class SVX_DLLPUBLIC SvxGrafBlueToolBoxControl : public SvxGrafToolBoxControl
{
public:
                        SFX_DECL_TOOLBOX_CONTROL();
                        SvxGrafBlueToolBoxControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
};

// ----------------------------------
// - SvxGrafLuminanceToolBoxControl -
// ----------------------------------

class SVX_DLLPUBLIC SvxGrafLuminanceToolBoxControl : public SvxGrafToolBoxControl
{
public:
                        SFX_DECL_TOOLBOX_CONTROL();
                        SvxGrafLuminanceToolBoxControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
};

// ---------------------------------
// - SvxGrafContrastToolBoxControl -
// ---------------------------------

class SVX_DLLPUBLIC SvxGrafContrastToolBoxControl : public SvxGrafToolBoxControl
{
public:
                        SFX_DECL_TOOLBOX_CONTROL();
                        SvxGrafContrastToolBoxControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
};

// ------------------------------
// - SvxGrafGammaToolBoxControl -
// ------------------------------

class SVX_DLLPUBLIC SvxGrafGammaToolBoxControl : public SvxGrafToolBoxControl
{
public:
                        SFX_DECL_TOOLBOX_CONTROL();
                        SvxGrafGammaToolBoxControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
};

// -------------------------------------
// - SvxGrafTransparenceToolBoxControl -
// -------------------------------------

class SVX_DLLPUBLIC SvxGrafTransparenceToolBoxControl : public SvxGrafToolBoxControl
{
public:
                        SFX_DECL_TOOLBOX_CONTROL();
                        SvxGrafTransparenceToolBoxControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
};

// -----------------------------
// - SvxGrafModeToolBoxControl -
// -----------------------------

class SVX_DLLPUBLIC SvxGrafModeToolBoxControl : public SfxToolBoxControl, public SfxListener
{
public:
                        SFX_DECL_TOOLBOX_CONTROL();
                        SvxGrafModeToolBoxControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
                        ~SvxGrafModeToolBoxControl();

    virtual void        StateChanged( USHORT nSID, SfxItemState eState, const SfxPoolItem* pState );
    virtual Window*     CreateItemWindow( Window *pParent );
};

// ---------------------
// - SvxGrafAttrHelper -
// ---------------------

class SdrView;
class SfxRequest;

class SVX_DLLPUBLIC SvxGrafAttrHelper
{
public:

    static void     ExecuteGrafAttr( SfxRequest& rReq, SdrView& rView );
    static void     GetGrafAttrState( SfxItemSet& rSet, SdrView& rView );
};

#endif // _SVX_GRAFCTRL_HXX
