/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: extrusioncontrols.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-06 07:27:43 $
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
#ifndef _SVX_EXTRUSION_CONTROLS_HXX
#define _SVX_EXTRUSION_CONTROLS_HXX

#ifndef _VALUESET_HXX //autogen
#include <svtools/valueset.hxx>
#endif
#ifndef _SFXLSTNER_HXX //autogen
#include <svtools/lstner.hxx>
#endif
#ifndef _SFXTBXCTRL_HXX //autogen
#include <sfx2/tbxctrl.hxx>
#endif
#ifndef _SVTREEBOX_HXX
#include <svtools/svtreebx.hxx>
#endif
#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#ifndef _SV_DIALOG_HXX
#include <vcl/dialog.hxx>
#endif
#ifndef _SV_FIELD_HXX
#include <vcl/field.hxx>
#endif

#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

#ifndef INCLUDED_SVXDLLAPI_H
#include "svx/svxdllapi.h"
#endif

class SfxBindings;
class ToolbarMenu;
class SfxStatusForwarder;

//========================================================================

namespace svx
{
class ToolboxButtonColorUpdater;

class ExtrusionDirectionWindow : public SfxPopupWindow
{
    using FloatingWindow::StateChanged;

private:
    ToolbarMenu*        mpMenu;
    ValueSet*           mpDirectionSet;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > mxFrame;

    SfxStatusForwarder* mpDirectionForewarder;
    SfxStatusForwarder* mpProjectionForewarder;

    Image       maImgDirection[9];
    Image       maImgDirectionH[9];

    Image       maImgPerspective;
    Image       maImgPerspectiveH;
    Image       maImgParallel;
    Image       maImgParallelH;

    bool        mbPopupMode;

    DECL_LINK( SelectHdl, void * );
    void            FillValueSet();

    void implSetDirection( sal_Int32 nSkew, bool bEnabled = true );
    void implSetProjection( sal_Int32 nProjection, bool bEnabled = true );
    void implInit();

protected:
    virtual BOOL    Close();
    virtual void    PopupModeEnd();

    /** This function is called when the window gets the focus.  It grabs
        the focus to the line ends value set so that it can be controlled with
        the keyboard.
    */
    virtual void GetFocus (void);

public:
    ExtrusionDirectionWindow( USHORT nId,
                              const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame );
    ExtrusionDirectionWindow( USHORT nId,
                              const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame,
                              Window* pParentWindow );
    ~ExtrusionDirectionWindow();

    void            StartSelection();

    virtual SfxPopupWindow* Clone() const;

    virtual void StateChanged( USHORT nSID, SfxItemState eState, const SfxPoolItem* pState );
    virtual void DataChanged( const DataChangedEvent& rDCEvt );
};

//========================================================================

class SVX_DLLPUBLIC ExtrusionDirectionControl : public SfxToolBoxControl
{
public:
    SFX_DECL_TOOLBOX_CONTROL();
    ExtrusionDirectionControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
    ~ExtrusionDirectionControl();

    virtual void                StateChanged( USHORT nSID, SfxItemState eState,
                                              const SfxPoolItem* pState );
    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual SfxPopupWindow*     CreatePopupWindow();
};

//========================================================================

class ExtrusionDepthWindow : public SfxPopupWindow
{
    using FloatingWindow::StateChanged;

private:
    ToolbarMenu* mpMenu;

    Image maImgDepth0;
    Image maImgDepth1;
    Image maImgDepth2;
    Image maImgDepth3;
    Image maImgDepth4;
    Image maImgDepthInfinity;

    Image maImgDepth0h;
    Image maImgDepth1h;
    Image maImgDepth2h;
    Image maImgDepth3h;
    Image maImgDepth4h;
    Image maImgDepthInfinityh;

    SfxStatusForwarder* mpDepthForewarder;
    SfxStatusForwarder* mpMetricForewarder;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > mxFrame;

    bool        mbPopupMode;
    FieldUnit   meUnit;
    double      mfDepth;
    bool        mbEnabled;

    DECL_LINK( SelectHdl, void * );

    void    implFillStrings( FieldUnit eUnit );
    void    implSetDepth( double fDepth, bool bEnabled );
    void    implInit();

protected:
    virtual BOOL    Close();
    virtual void    PopupModeEnd();

    /** This function is called when the window gets the focus.  It grabs
        the focus to the line ends value set so that it can be controlled with
        the keyboard.
    */
    virtual void GetFocus (void);

public:
    ExtrusionDepthWindow( USHORT nId,
                          const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame );
    ExtrusionDepthWindow( USHORT nId,
                          const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame,
                          Window* pParentWindow );
    ~ExtrusionDepthWindow();

    void            StartSelection();

    virtual SfxPopupWindow* Clone() const;

    virtual void StateChanged( USHORT nSID, SfxItemState eState, const SfxPoolItem* pState );
    virtual void DataChanged( const DataChangedEvent& rDCEvt );
};

//========================================================================

class SVX_DLLPUBLIC ExtrusionDepthControl : public SfxToolBoxControl
{
public:
    SFX_DECL_TOOLBOX_CONTROL();
    ExtrusionDepthControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
    ~ExtrusionDepthControl();

    virtual void                StateChanged( USHORT nSID, SfxItemState eState,
                                              const SfxPoolItem* pState );
    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual SfxPopupWindow*     CreatePopupWindow();
};

//========================================================================

class ExtrusionLightingWindow : public SfxPopupWindow
{
    using FloatingWindow::StateChanged;

private:
    ToolbarMenu* mpMenu;
    ValueSet*    mpLightingSet;

    Image maImgLightingOff[9];
    Image maImgLightingOn[9];
    Image maImgLightingPreview[9];

    Image maImgLightingOffh[9];
    Image maImgLightingOnh[9];
    Image maImgLightingPreviewh[9];

    Image maImgBright;
    Image maImgNormal;
    Image maImgDim;
    Image maImgBrighth;
    Image maImgNormalh;
    Image maImgDimh;

    SfxStatusForwarder* mpLightingDirectionForewarder;
    SfxStatusForwarder* mpLightingIntensityForewarder;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > mxFrame;

    bool    mbPopupMode;
    int     mnLevel;
    bool    mbLevelEnabled;
    int     mnDirection;
    bool    mbDirectionEnabled;

    void    implSetIntensity( int nLevel, bool bEnabled );
    void    implSetDirection( int nDirection, bool bEnabled );
    void    implInit();

    DECL_LINK( SelectHdl, void * );

protected:
    virtual BOOL    Close();
    virtual void    PopupModeEnd();

    /** This function is called when the window gets the focus.  It grabs
        the focus to the line ends value set so that it can be controlled with
        the keyboard.
    */
    virtual void GetFocus (void);

public:
    ExtrusionLightingWindow( USHORT nId,
                             const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame );
    ExtrusionLightingWindow( USHORT nId,
                             const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame,
                             Window* pParentWindow );
    ~ExtrusionLightingWindow();

    void            StartSelection();

    virtual SfxPopupWindow* Clone() const;

    virtual void StateChanged( USHORT nSID, SfxItemState eState, const SfxPoolItem* pState );
    virtual void DataChanged( const DataChangedEvent& rDCEvt );
};

//========================================================================

class SVX_DLLPUBLIC ExtrusionLightingControl : public SfxToolBoxControl
{
public:
    SFX_DECL_TOOLBOX_CONTROL();
    ExtrusionLightingControl( USHORT nSlotid, USHORT nId, ToolBox& rTbx );
    ~ExtrusionLightingControl();

    virtual void                StateChanged( USHORT nSID, SfxItemState eState,
                                              const SfxPoolItem* pState );
    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual SfxPopupWindow*     CreatePopupWindow();
};

//========================================================================

class ExtrusionSurfaceWindow : public SfxPopupWindow
{
    using FloatingWindow::StateChanged;
private:
    ToolbarMenu* mpMenu;

    Image maImgSurface1;
    Image maImgSurface2;
    Image maImgSurface3;
    Image maImgSurface4;
    Image maImgSurface1h;
    Image maImgSurface2h;
    Image maImgSurface3h;
    Image maImgSurface4h;

    SfxStatusForwarder* mpSurfaceForewarder;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > mxFrame;

    bool        mbPopupMode;

    DECL_LINK( SelectHdl, void * );

    void    implSetSurface( int nSurface, bool bEnabled );
    void    implInit();

protected:
    virtual BOOL    Close();
    virtual void    PopupModeEnd();

    /** This function is called when the window gets the focus.  It grabs
        the focus to the line ends value set so that it can be controlled with
        the keyboard.
    */
    virtual void GetFocus (void);

public:
    ExtrusionSurfaceWindow( USHORT nId,
                            const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame );
    ExtrusionSurfaceWindow( USHORT nId,
                            const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame,
                            Window* pParentWindow );
    ~ExtrusionSurfaceWindow();

    void            StartSelection();

    virtual SfxPopupWindow* Clone() const;

    virtual void StateChanged( USHORT nSID, SfxItemState eState, const SfxPoolItem* pState );
    virtual void DataChanged( const DataChangedEvent& rDCEvt );
};

//========================================================================

class SVX_DLLPUBLIC ExtrusionSurfaceControl : public SfxToolBoxControl
{
public:
    SFX_DECL_TOOLBOX_CONTROL();
    ExtrusionSurfaceControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
    ~ExtrusionSurfaceControl();

    virtual void                StateChanged( USHORT nSID, SfxItemState eState,
                                              const SfxPoolItem* pState );
    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual SfxPopupWindow*     CreatePopupWindow();
};

//========================================================================

class SVX_DLLPUBLIC ExtrusionColorControl : public SfxToolBoxControl
{
private:
    ToolboxButtonColorUpdater*  mpBtnUpdater;

public:
    SFX_DECL_TOOLBOX_CONTROL();
    ExtrusionColorControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
    ~ExtrusionColorControl();

    virtual void                StateChanged( USHORT nSID, SfxItemState eState,
                                              const SfxPoolItem* pState );
    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual SfxPopupWindow*     CreatePopupWindow();
};

//========================================================================

class ExtrusionDepthDialog : public ModalDialog
{
    FixedText           maFLDepth;
    MetricField         maMtrDepth;
    OKButton            maOKButton;
    CancelButton        maCancelButton;
    HelpButton          maHelpButton;

    FieldUnit           meDefaultUnit;

public:
    ExtrusionDepthDialog( Window* pParent, double fDepth, FieldUnit eDefaultUnit );
    ~ExtrusionDepthDialog();

    double getDepth() const;
};

}
#endif

