/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: workwin.hxx,v $
 *
 *  $Revision: 1.23 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 22:35:04 $
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
#ifndef _SFXWORKWIN_HXX
#define _SFXWORKWIN_HXX

#include <vector>

#ifndef _COM_SUN_STAR_FRAME_XDISPATCH_HPP_
#include <com/sun/star/frame/XDispatch.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_XUILEMENT_HPP_
#include <com/sun/star/ui/XUIElement.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XSTATUSINDICATOR_HPP_
#include <com/sun/star/task/XStatusIndicator.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XLAYOUTMANAGERLISTENER_HPP_
#include <com/sun/star/frame/XLayoutManagerListener.hpp>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

#ifndef _CPPUHELPER_PROPSHLP_HXX
#include <cppuhelper/propshlp.hxx>
#endif

#define _SVSTDARR_USHORTS
#include <svtools/svstdarr.hxx>     // SvUShorts

#ifndef _RTL_USTRING_
#include <rtl/ustring.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif

#include "sfx.hrc"
#include "childwin.hxx"
#include "shell.hxx"
#include "minarray.hxx"
#include "ctrlitem.hxx"
#include "viewfrm.hxx"

class SfxInPlaceEnv_Impl;
class SfxPlugInEnv_Impl;
class SfxSplitWindow;
class SfxWorkWindow;

//====================================================================
// Dieser struct h"alt alle relevanten Informationen "uber Toolboxen bereit.

struct SfxObjectBar_Impl
{
    USHORT        nId;   // Resource - und ConfigId der Toolbox
    USHORT        nMode; // spezielle Sichtbarkeitsflags
    USHORT        nPos;
    USHORT        nIndex;
    sal_Bool      bDestroy;
    String        aName;
    SfxInterface* pIFace;

    SfxObjectBar_Impl() :
        nId(0),
        nMode(0),
        bDestroy(sal_False),
        pIFace(0)
    {}
};

//------------------------------------------------------------------------------
// Dieser struct h"alt alle relevanten Informationen "uber die Statuszeile bereit.
struct SfxStatBar_Impl
{
    USHORT                  nId;
    BOOL                    bOn;
    BOOL                    bTemp;

    SfxStatBar_Impl() :
        nId(0),
        bOn(TRUE),
        bTemp(FALSE)
    {}
};

//------------------------------------------------------------------------------

#define CHILD_NOT_VISIBLE   0
#define CHILD_ACTIVE        1   // nicht durch HidePopups ausgeschaltet
#define CHILD_NOT_HIDDEN    2   // nicht durch HideChildWindow ausgeschaltet
#define CHILD_FITS_IN       4   // nicht zu gro\s f"ur OutputSize des parent
#define CHILD_VISIBLE       (CHILD_NOT_HIDDEN | CHILD_ACTIVE | CHILD_FITS_IN)
#define CHILD_ISVISIBLE     (CHILD_NOT_HIDDEN | CHILD_ACTIVE)

struct SfxChild_Impl
{
    Window*                         pWin;
    Size                            aSize;
    SfxChildAlignment               eAlign;
    USHORT                          nVisible;
    BOOL                            bResize;
    BOOL                            bCanGetFocus;
    BOOL                            bSetFocus;

    SfxChild_Impl( Window& rChild, const Size& rSize,
                   SfxChildAlignment eAlignment, BOOL bIsVisible ):
        pWin(&rChild), aSize(rSize), eAlign(eAlignment), bResize(FALSE),
        bCanGetFocus( FALSE ), bSetFocus( FALSE )
    {
        nVisible = bIsVisible ? CHILD_VISIBLE : CHILD_NOT_VISIBLE;
    }
};

//--------------------------------------------------------------------
class SfxChildWinController_Impl : public SfxControllerItem
{
    SfxWorkWindow*  pWorkwin;

    public:
                    SfxChildWinController_Impl( USHORT nId, SfxWorkWindow *pWin );
    virtual void    StateChanged( USHORT nSID, SfxItemState eState,
                            const SfxPoolItem* pState );
};

struct SfxChildWin_Impl
{
    USHORT                          nSaveId;        // die ChildWindow-Id
    USHORT                          nInterfaceId;   // der aktuelle Context
    USHORT                          nId;            // aktuelle Id
    SfxChildWindow*                 pWin;
    BOOL                            bCreate;
    SfxChildWinInfo                 aInfo;
    SfxChild_Impl*                  pCli;           // != 0 bei direkten Children
    USHORT                          nVisibility;
    BOOL                            bEnable;
    SfxChildWinController_Impl*     pControl;
    BOOL                            bDisabled;

    SfxChildWin_Impl( ULONG nID ) :
        nSaveId((USHORT) (nID & 0xFFFF) ),
        nInterfaceId((USHORT) (nID >> 16)),
        nId(nSaveId),
        pWin(0),
        bCreate(FALSE),
        pCli(0),
        nVisibility( FALSE ),
        bEnable( TRUE ),
        pControl( 0 ),
        bDisabled( FALSE )
    {}
};

enum SfxChildIdentifier
{
    SFX_CHILDWIN_STATBAR,
    SFX_CHILDWIN_OBJECTBAR,
    SFX_CHILDWIN_DOCKINGWINDOW,
    SFX_CHILDWIN_SPLITWINDOW
};

enum SfxDockingConfig
{
    SFX_SETDOCKINGRECTS,
    SFX_ALIGNDOCKINGWINDOW,
    SFX_TOGGLEFLOATMODE,
    SFX_MOVEDOCKINGWINDOW
};

DECL_PTRARRAY( SfxChildList_Impl, SfxChild_Impl*, 2, 2 )
DECL_PTRARRAY( SfxChildWindows_Impl, SfxChildWin_Impl*, 2, 2 )

SV_DECL_OBJARR( SfxObjectBarArr_Impl, SfxObjectBar_Impl, 1, 2 )

struct SfxObjectBarList_Impl
{
    SfxObjectBarArr_Impl    aArr;
    USHORT                  nAct;

    SfxObjectBar_Impl       operator[] ( USHORT n )
                            { return aArr[n]; }
    SfxObjectBar_Impl       Actual()
                            { return aArr[nAct]; }
};

struct SfxSplitWin_Impl
{
    SfxSplitWindow*         pSplitWin;
    SfxChildWindows_Impl*   pChildWins;
};

#define SFX_SPLITWINDOWS_LEFT   0
#define SFX_SPLITWINDOWS_TOP    2
#define SFX_SPLITWINDOWS_RIGHT  1
#define SFX_SPLITWINDOWS_BOTTOM 3
#define SFX_SPLITWINDOWS_MAX    4

//--------------------------------------------------------------------

class LayoutManagerListener : public ::com::sun::star::frame::XLayoutManagerListener,
                              public ::com::sun::star::lang::XTypeProvider,
                              public ::com::sun::star::lang::XComponent,
                              public ::cppu::OWeakObject
{
    public:
        LayoutManagerListener( SfxWorkWindow* pWrkWin );
        virtual ~LayoutManagerListener();

        SFX_DECL_XINTERFACE_XTYPEPROVIDER

        void setFrame( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame );

        //---------------------------------------------------------------------------------------------------------
        //  XComponent
        //---------------------------------------------------------------------------------------------------------
        virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL dispose() throw( ::com::sun::star::uno::RuntimeException );

        //---------------------------------------------------------------------------------------------------------
        //  XEventListener
        //---------------------------------------------------------------------------------------------------------
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& aEvent ) throw( ::com::sun::star::uno::RuntimeException );

        //---------------------------------------------------------------------------------------------------------
        // XLayoutManagerEventListener
        //---------------------------------------------------------------------------------------------------------
        virtual void SAL_CALL layoutEvent( const ::com::sun::star::lang::EventObject& aSource, ::sal_Int16 eLayoutEvent, const ::com::sun::star::uno::Any& aInfo ) throw (::com::sun::star::uno::RuntimeException);

    private:
        sal_Bool                                                                m_bHasFrame;
        SfxWorkWindow*                                                          m_pWrkWin;
        ::com::sun::star::uno::WeakReference< ::com::sun::star::frame::XFrame > m_xFrame;
        rtl::OUString                                                           m_aLayoutManagerPropName;
};

class SfxWorkWindow
{
    friend class UIElementWrapper;

protected:
    SvUShorts               aSortedList;
    SfxStatBar_Impl         aStatBar;
    std::vector< SfxObjectBar_Impl > aObjBarList;
    Rectangle               aClientArea;
    Rectangle               aUpperClientArea;
    SfxWorkWindow*          pParent;
    SfxSplitWindow*         pSplit[SFX_SPLITWINDOWS_MAX];
    SfxChildList_Impl*      pChilds;
    SfxChildWindows_Impl*   pChildWins;
    SfxBindings*            pBindings;
    Window*                 pWorkWin;
    SfxShell*               pConfigShell;
    Window*                 pActiveChild;
    USHORT                  nUpdateMode;
    USHORT                  nChilds;
    USHORT                  nOrigMode;
    BOOL                    bSorted : 1;
    BOOL                    bDockingAllowed : 1;
    BOOL                    bInternalDockingAllowed : 1;
    BOOL                    bAllChildsVisible : 1;
    BOOL                    bIsFullScreen : 1;
    BOOL                    bShowStatusBar : 1;
    BOOL                    bLocked : 1;
    rtl::OUString           m_aStatusBarResName;
    rtl::OUString           m_aLayoutManagerPropName;
    rtl::OUString           m_aTbxTypeName;
    rtl::OUString           m_aProgressBarResName;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent > m_xLayoutManagerListener;

protected:
    void                    CreateChildWin_Impl(SfxChildWin_Impl*,BOOL);
    void                    RemoveChildWin_Impl(SfxChildWin_Impl*);
    void                    Sort_Impl();
    void                    AlignChild_Impl( Window& rWindow, const Size& rNewSize,
                                     SfxChildAlignment eAlign );
    SfxChild_Impl*          FindChild_Impl( const Window& rWindow ) const;
    virtual BOOL            RequestTopToolSpacePixel_Impl( SvBorder aBorder );
    virtual Rectangle       GetTopRect_Impl();
    SvBorder                Arrange_Impl();
    virtual void            SaveStatus_Impl(SfxChildWindow*, const SfxChildWinInfo&);
    static sal_Bool         IsPluginMode( SfxObjectShell* pObjShell );

public:
                            SfxWorkWindow( Window *pWin, SfxBindings& rBindings, SfxWorkWindow* pParent = NULL);
    virtual                 ~SfxWorkWindow();
    SystemWindow*           GetTopWindow() const;
    SfxBindings&            GetBindings()
                            { return *pBindings; }
    Window*                 GetWindow() const
                            { return pWorkWin; }
    Rectangle               GetFreeArea( BOOL bAutoHide ) const;
    void                    SetDockingAllowed(BOOL bSet)
                            { bDockingAllowed = bSet; }
    void                    SetInternalDockingAllowed(BOOL bSet)
                            { bInternalDockingAllowed = bSet; }
    BOOL                    IsDockingAllowed() const
                            { return bDockingAllowed; }
    BOOL                    IsInternalDockingAllowed() const
                            { return bInternalDockingAllowed; }
    SfxWorkWindow*          GetParent_Impl() const
                            { return pParent; }
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >          CreateDispatch( const String& );

    // Methoden f"ur alle Child-Fenster
    void                    DataChanged_Impl( const DataChangedEvent& rDCEvt );
    void                    ReleaseChild_Impl( Window& rWindow );
    SfxChild_Impl*          RegisterChild_Impl( Window& rWindow, SfxChildAlignment eAlign, BOOL bCanGetFocus=FALSE );
    void                    ShowChilds_Impl();
    void                    HideChilds_Impl();
    void                    Close_Impl();
    BOOL                    PrepareClose_Impl();
    virtual void            ArrangeChilds_Impl();
    void                    DeleteControllers_Impl();
    void                    SaveStatus_Impl();
    void                    HidePopups_Impl(BOOL bHide, BOOL bParent=FALSE, USHORT nId=0);
    void                    ConfigChild_Impl(SfxChildIdentifier,
                                             SfxDockingConfig, USHORT);
    void                    MakeChildsVisible_Impl( BOOL bVis );
    void                    ArrangeAutoHideWindows( SfxSplitWindow *pSplit );
    BOOL                    IsAutoHideMode( const SfxSplitWindow *pSplit );
    void                    EndAutoShow_Impl( Point aPos );
    void                    SetFullScreen_Impl( BOOL bSet ) { bIsFullScreen = bSet; }
    BOOL                    IsFullScreen_Impl() const { return bIsFullScreen; }

    // Methoden f"ur Objectbars
    virtual void            UpdateObjectBars_Impl();
    void                    ResetObjectBars_Impl();
    void                    SetObjectBar_Impl( USHORT nPos, const ResId& rId,
                                    SfxInterface *pIFace, const String* pName=0 );
    Window*                 GetObjectBar_Impl( USHORT nPos, ResId& rResId );
    FASTBOOL                KnowsObjectBar_Impl( USHORT nPos ) const;
    BOOL                    IsVisible_Impl();
    void                    MakeVisible_Impl( BOOL );
    void                    SetObjectBarVisibility_Impl( USHORT nVis );
    BOOL                    IsContainer_Impl() const;
    void                    Lock_Impl( BOOL );
    void                    NextObjectBar_Impl( USHORT nPos );
    USHORT                  HasNextObjectBar_Impl( USHORT nPos, String* pStr=0 );
    void                    SetObjectBarCustomizeMode_Impl( BOOL );

    // Methoden f"ur ChildWindows
    void                    UpdateChildWindows_Impl();
    void                    ResetChildWindows_Impl();
    void                    SetChildWindowVisible_Impl( ULONG, BOOL, USHORT );
    void                    ToggleChildWindow_Impl(USHORT,BOOL);
    BOOL                    HasChildWindow_Impl(USHORT);
    BOOL                    KnowsChildWindow_Impl(USHORT);
    void                    ShowChildWindow_Impl(USHORT, BOOL bVisible, BOOL bSetFocus);
    void                    SetChildWindow_Impl(USHORT, BOOL bOn, BOOL bSetFocus);
    SfxChildWindow*         GetChildWindow_Impl(USHORT);
    virtual void            InitializeChild_Impl(SfxChildWin_Impl*);
    SfxSplitWindow*         GetSplitWindow_Impl(SfxChildAlignment);

    BOOL                    IsVisible_Impl( USHORT nMode ) const;
    void                    DisableChildWindow_Impl( USHORT nId, BOOL bDisable );
    BOOL                    IsFloating( USHORT nId );
    void                    ChangeWindow_Impl( Window *pNew );
    void                    SetActiveChild_Impl( Window *pChild );
    Window*                 GetActiveChild_Impl();
    virtual BOOL            ActivateNextChild_Impl( BOOL bForward = TRUE );

    // Methoden f"ur StatusBar
    void                    SetTempStatusBar_Impl( BOOL bSet );
    void                    ResetStatusBar_Impl();
    void                    SetStatusBar_Impl(const ResId&, SfxShell *pShell, SfxBindings& );
    void                    UpdateStatusBar_Impl();
    ::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator > GetStatusIndicator();
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > GetFrameInterface();
};
/*
class SfxPIWorkWin_Impl : public SfxWorkWindow
{
    SfxPlugInEnv_Impl*  pEnv;
    Window*             pDialogParent;

public:
                        SfxPIWorkWin_Impl(
                            Window *pWin, SfxBindings &rBindings,
                            SfxPlugInEnv_Impl*  pE);
    virtual             Window* GetModalDialogParent() const;

private:
    virtual void        ArrangeChilds_Impl();
    virtual void        UpdateObjectBars_Impl();
    virtual void        SaveStatus_Impl(SfxChildWindow*, const SfxChildWinInfo&);
    virtual void        InitializeChild_Impl(SfxChildWin_Impl*);
    virtual Rectangle   GetTopRect_Impl();
};


class SfxIPWorkWin_Impl : public SfxWorkWindow
{
    SfxInPlaceEnv_Impl* pEnv;

public:
                        SfxIPWorkWin_Impl( WorkWindow *pWin, SfxBindings &rBindings,
                                SfxInPlaceEnv_Impl* pE);
private:
    virtual BOOL        RequestTopToolSpacePixel_Impl( SvBorder aBorder );
    virtual void        UpdateObjectBars_Impl();
    virtual Rectangle   GetTopRect_Impl();
    virtual void        ArrangeChilds_Impl();
    virtual void        SaveStatus_Impl(SfxChildWindow*, const SfxChildWinInfo&);
};
*/
class SfxFrameWorkWin_Impl : public SfxWorkWindow
{
    SfxFrame*           pMasterFrame;
    SfxFrame*           pFrame;
public:
                        SfxFrameWorkWin_Impl( Window* pWin, SfxFrame* pFrm, SfxFrame* pMaster );
    virtual void        ArrangeChilds_Impl();
    virtual void        UpdateObjectBars_Impl();
    virtual Rectangle   GetTopRect_Impl();
};


#endif
