/*************************************************************************
 *
 *  $RCSfile: fontworkgallery.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: kz $ $Date: 2005-01-21 14:37:39 $
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


#ifndef _SVX_FONTWORK_GALLERY_DIALOG_HXX
#define _SVX_FONTWORK_GALLERY_DIALOG_HXX

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _VALUESET_HXX //autogen
#include <svtools/valueset.hxx>
#endif
#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#ifndef _SV_DIALOG_HXX
#include <vcl/dialog.hxx>
#endif
#ifndef _SFXTBXCTRL_HXX //autogen
#include <sfx2/tbxctrl.hxx>
#endif

#ifndef INCLUDED_SVXDLLAPI_H
#include "svx/svxdllapi.h"
#endif

#include <vector>

class FmFormModel;
class SdrView;
class Window;
class SdrTextObj;
class SdrObject;
class SdrModel;

class SfxBindings;
class ToolbarMenu;
class SfxStatusForwarder;
class SvxTbxButtonColorUpdater_Impl;

//------------------------------------------------------------------------

namespace svx
{


class FontWorkAlignmentWindow : public SfxPopupWindow
{
private:
    ToolbarMenu* mpMenu;

    Image maImgAlgin1;
    Image maImgAlgin2;
    Image maImgAlgin3;
    Image maImgAlgin4;
    Image maImgAlgin5;
    Image maImgAlgin1h;
    Image maImgAlgin2h;
    Image maImgAlgin3h;
    Image maImgAlgin4h;
    Image maImgAlgin5h;

    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > mxFrame;

    bool        mbPopupMode;

    DECL_LINK( SelectHdl, void * );

    void    implSetAlignment( int nAlignmentMode, bool bEnabled );

protected:
    virtual BOOL    Close();
    virtual void    PopupModeEnd();

    /** This function is called when the window gets the focus.  It grabs
        the focus to the line ends value set so that it can be controlled with
        the keyboard.
    */
    virtual void GetFocus (void);

public:
    FontWorkAlignmentWindow( USHORT nId,
                            const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame );
    ~FontWorkAlignmentWindow();

    void            StartSelection();

    virtual SfxPopupWindow* Clone() const;

    virtual void StateChanged( USHORT nSID, SfxItemState eState, const SfxPoolItem* pState );
    virtual void DataChanged( const DataChangedEvent& rDCEvt );
};

class SVX_DLLPUBLIC FontWorkAlignmentControl : public SfxToolBoxControl
{
public:
    SFX_DECL_TOOLBOX_CONTROL();
    FontWorkAlignmentControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
    ~FontWorkAlignmentControl();

    virtual void                StateChanged( USHORT nSID, SfxItemState eState,
                                              const SfxPoolItem* pState );
    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual SfxPopupWindow*     CreatePopupWindow();
};

//------------------------------------------------------------------------

class FontWorkCharacterSpacingWindow : public SfxPopupWindow
{
private:
    ToolbarMenu* mpMenu;

    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > mxFrame;

    bool        mbPopupMode;

    DECL_LINK( SelectHdl, void * );

    void    implSetCharacterSpacing( sal_Int32 nCharacterSpacing, bool bEnabled );
    void    implSetKernCharacterPairs( sal_Bool bKernOnOff, bool bEnabled );

protected:
    virtual BOOL    Close();
    virtual void    PopupModeEnd();

    /** This function is called when the window gets the focus.  It grabs
        the focus to the line ends value set so that it can be controlled with
        the keyboard.
    */
    virtual void GetFocus (void);

public:
    FontWorkCharacterSpacingWindow( USHORT nId,
                            const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame );
    ~FontWorkCharacterSpacingWindow();

    void            StartSelection();

    virtual SfxPopupWindow* Clone() const;

    virtual void StateChanged( USHORT nSID, SfxItemState eState, const SfxPoolItem* pState );
    virtual void DataChanged( const DataChangedEvent& rDCEvt );
};

class SVX_DLLPUBLIC FontWorkCharacterSpacingControl : public SfxToolBoxControl
{
public:
    SFX_DECL_TOOLBOX_CONTROL();
    FontWorkCharacterSpacingControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
    ~FontWorkCharacterSpacingControl();

    virtual void                StateChanged( USHORT nSID, SfxItemState eState,
                                              const SfxPoolItem* pState );
    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual SfxPopupWindow*     CreatePopupWindow();
};

//------------------------------------------------------------------------

class SVX_DLLPUBLIC FontWorkShapeTypeControl : public SfxToolBoxControl
{
public:
    SFX_DECL_TOOLBOX_CONTROL();
    FontWorkShapeTypeControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
    ~FontWorkShapeTypeControl();

    virtual void                Select( BOOL bMod1 = FALSE );
    virtual void                StateChanged( USHORT nSID, SfxItemState eState,
                                              const SfxPoolItem* pState );
    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual SfxPopupWindow*     CreatePopupWindow();
};

//------------------------------------------------------------------------

class FontworkCharacterSpacingDialog : public ModalDialog
{
    FixedText           maFLScale;
    MetricField         maMtrScale;
    OKButton            maOKButton;
    CancelButton        maCancelButton;
    HelpButton          maHelpButton;

public:
    FontworkCharacterSpacingDialog( Window* pParent, sal_Int32 nScale );
    ~FontworkCharacterSpacingDialog();

    sal_Int32 getScale() const;
};


class SVX_DLLPUBLIC FontWorkGalleryDialog : public ModalDialog
{
    ValueSet            maCtlFavorites;
    FixedLine           maFLFavorites;
    OKButton            maOKButton;
    CancelButton        maCancelButton;
    HelpButton          maHelpButton;

    sal_uInt16          mnThemeId;

    SdrView*            mpSdrView;
    FmFormModel*        mpModel;

    String              maStrClickToAddText;

    DECL_LINK( DoubleClickFavoriteHdl, void * );
    DECL_LINK( ClickOKHdl, void * );
    DECL_LINK( ClickTextDirectionHdl, ImageButton * );

    SdrObject**         mppSdrObject;
    SdrModel*           mpDestModel;

    void            initfavorites(sal_uInt16 nThemeId, std::vector< Bitmap * >& rFavorites);
    void            insertSelectedFontwork();
    void            changeText( SdrTextObj* pObj );
    void            fillFavorites( sal_uInt16 nThemeId, std::vector< Bitmap * >& rFavorites );

    std::vector< Bitmap * > maFavoritesHorizontal;

public:
    FontWorkGalleryDialog( SdrView* pView, Window* pParent, sal_uInt16 nSID );
    ~FontWorkGalleryDialog();

    // SJ: if the SdrObject** is set, the SdrObject is not inserted into the page when executing the dialog
    void SetSdrObjectRef( SdrObject**, SdrModel* pModel );
};

};

#endif
