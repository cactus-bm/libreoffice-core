/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SlideSorterViewShell.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: obo $ $Date: 2006-03-21 17:27:10 $
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

#ifndef SD_SLIDESORTER_SLIDE_SORTER_VIEW_SHELL_HXX
#define SD_SLIDESORTER_SLIDE_SORTER_VIEW_SHELL_HXX

#include "ViewShell.hxx"
#include "SlideSorterViewShell.hxx"
#include "glob.hxx"
#ifndef _SFX_SHELL_HXX
#include <sfx2/shell.hxx>
#endif
#ifndef _VIEWFAC_HXX
#include <sfx2/viewfac.hxx>
#endif

class ScrollBarBox;
class TabBar;
class Window;

namespace sd { namespace slidesorter { namespace model {
class SlideSorterModel;
} } }

namespace sd { namespace slidesorter { namespace view {
class SlideSorterView;
} } }

namespace sd { namespace slidesorter { namespace controller {
class Listener;
class SlideSorterController;
class SlotManager;
} } }

namespace sd { namespace slidesorter {


class SlideSorterViewShell
    : public ViewShell
{
    friend class controller::SlotManager;
public:
    TYPEINFO();
    SFX_DECL_INTERFACE(SD_IF_SDSLIDESORTERVIEWSHELL);

    enum TabBarEntry
    {
        TBE_SWITCH = 0,
        TBE_SLIDES = 1,
        TBE_MASTER_PAGES = 2
    };

    static SfxShell* CreateInstance (
        sal_Int32 nId,
        SfxShell* pParent,
        void* pUserData,
        ViewShellBase& rBase);

    SlideSorterViewShell (
        SfxViewFrame* pFrame,
        ViewShellBase& rViewShellBase,
        ::Window* pParentWindow,
        FrameView* pFrameView);

    virtual ~SlideSorterViewShell (void);

    /** Late initialization that has to be called after a new instance has
        completed its construction.
    */
    virtual void Init (bool bIsMainViewShell);

    /** Return a slide sorter that is currently displayed in one of the
        panes that belong to the given ViewShellBase object.
        When there is only one slide sorter visible then that one is
        returned.  When two (or more) are visible then the one in the center
        pane is returned.  When no slidesorter is visible then NULL is
        returned.
    */
    static SlideSorterViewShell* GetSlideSorter (ViewShellBase& rBase);

    virtual void GetFocus (void);
    virtual void LoseFocus (void);
    virtual SdPage* GetActualPage (void);

    /// inherited from sd::ViewShell
    virtual SdPage* getCurrentPage() const;

    void ExecCtrl (SfxRequest& rRequest);
    virtual void GetCtrlState (SfxItemSet &rSet);
    virtual void FuSupport (SfxRequest& rRequest);
    virtual void FuTemporary (SfxRequest& rRequest);
    virtual void GetStatusBarState (SfxItemSet& rSet);
    virtual void FuPermanent (SfxRequest& rRequest);
    void GetAttrState (SfxItemSet& rSet);
    void ExecStatusBar (SfxRequest& rRequest);
    virtual void Command (const CommandEvent& rEvent, ::sd::Window* pWindow);
    virtual void GetMenuState (SfxItemSet &rSet);

    virtual void ReadFrameViewData (FrameView* pView);
    virtual void WriteFrameViewData (void);

    /** Set the zoom factor.  The given value is clipped against an upper
        bound.
        @param nZoom
            An integer percent value, i.e. nZoom/100 is the actual zoom
            factor.
        */
    virtual void SetZoom (long int nZoom);
    virtual void SetZoomRect (const Rectangle& rZoomRect);

    /** This is a callback method used by the active window to delegate its
        Paint() call to.  This view shell itself delegates it to the view.
    */
    virtual void Paint(const Rectangle& rRect, ::sd::Window* pWin);

    /** Place and size the controls and windows.  You may want to call this
        method when something has changed that for instance affects the
        visibility state of the scroll bars.
    */
    virtual void ArrangeGUIElements (void);

    /** Return the control of the vertical scroll bar.
    */
    ScrollBar* GetVerticalScrollBar (void) const;

    /** Return the control of the horizontal scroll bar.
    */
    ScrollBar* GetHorizontalScrollBar (void) const;

    /** Return the scroll bar filler that paints the little square that is
        enclosed by the two scroll bars.
    */
    ScrollBarBox* GetScrollBarFiller (void) const;

    /** Set the tab bar to the given mode.
        @param eEntry
            When TBE_SWITCH is given, then switch between the two tabs.
    */
    TabBarEntry SwitchTabBar (TabBarEntry eEntry);

    controller::SlideSorterController& GetSlideSorterController (void);

    //===== Drag and Drop =====================================================

    virtual void StartDrag (
        const Point& rDragPt,
        ::Window* pWindow );
    virtual void DragFinished (
        sal_Int8 nDropAction);
    virtual sal_Int8 AcceptDrop (
        const AcceptDropEvent& rEvt,
        DropTargetHelper& rTargetHelper,
        ::sd::Window* pTargetWindow = NULL,
        USHORT nPage = SDRPAGE_NOTFOUND,
        USHORT nLayer = SDRPAGE_NOTFOUND );
    virtual sal_Int8 ExecuteDrop (
        const ExecuteDropEvent& rEvt,
        DropTargetHelper& rTargetHelper,
        ::sd::Window* pTargetWindow = NULL,
        USHORT nPage = SDRPAGE_NOTFOUND,
        USHORT nLayer = SDRPAGE_NOTFOUND);

    /** Return the selected pages by putting them into the given container.
        The container does not have to be empty.  It is not cleared.
    */
    void GetSelectedPages (::std::vector<SdPage*>& pPageContainer);

    /** Add a listener that is called when the selection of the slide sorter
        changes.
        @param rListener
            When this method is called multiple times for the same listener
            the second and all following calls are ignored.  Each listener
            is added only once.
    */
    void AddSelectionChangeListener (const Link& rListener);

    /** Remove a listener that was called when the selection of the slide
        sorter changes.
        @param rListener
            It is save to pass a listener that was not added are has been
            removed previously.  Such calls are ignored.
    */
    void RemoveSelectionChangeListener (const Link& rListener);

    virtual ::std::auto_ptr<DrawSubController> CreateSubController (void);

    /** Create an accessible object representing the specified window.
        @param pWindow
            The returned object makes the document displayed in this window
            accessible.
        @return
            Returns an <type>AccessibleSlideSorterView</type> object.
   */
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible>
        CreateAccessibleDocumentView (::sd::Window* pWindow);

protected:
    ::std::auto_ptr<controller::SlideSorterController> mpSlideSorterController;
    ::std::auto_ptr<model::SlideSorterModel> mpSlideSorterModel;
    ::std::auto_ptr<view::SlideSorterView> mpSlideSorterView;

    virtual SvBorder GetBorder (bool bOuterResize);

    /** This virtual method makes it possible to create a specialization of
        the slide sorter view shell that works with its own implementation
        of model, view, and controller.  The default implementation simply
        calls the CreateModel(), CreateView(), and CreateController()
        methods in this order.
    */
    virtual void CreateModelViewController (void);

    /** Create the model for the view shell.  When called from the default
        implementation of CreateModelViewController() then neither view nor
        controller do exist.  Test their pointers when in doubt.
    */
    virtual model::SlideSorterModel* CreateModel (void);

    /** Create the view for the view shell.  When called from the default
        implementation of CreateModelViewController() then the model but not
        the controller does exist.  Test their pointers when in doubt.
    */
    virtual view::SlideSorterView* CreateView (void);

    /** Create the controller for the view shell.  When called from the default
        implementation of CreateModelViewController() then both the view and
        the controller do exist.  Test their pointers when in doubt.
    */
    virtual controller::SlideSorterController* CreateController (void);

    /** This method is overloaded to handle a missing tool bar correctly.
        This is the case when the slide sorter is not the main view shell.
    */
    virtual SfxUndoManager* ImpGetUndoManager (void) const;

private:
    ::std::auto_ptr<TabBar> mpTabBar;

    /** Set this flag to <TRUE/> to force a layout before the next paint.
    */
    bool mbLayoutPending;

    /** Create the controls for the slide sorter.  This are the tab bar
       for switching the edit mode, the scroll bar, and the actual
       slide sorter view window.
       This method is usually called exactly one time from the
       constructor.
    */
    void SetupControls (::Window* pParentWindow);

    /** This method is usually called exactly one time from the
        constructor.
    */
    void SetupListeners (void);

    /** Release the listeners that have been installed in SetupListeners().
    */
    void ReleaseListeners (void);

    /** This method overwrites the one from our base class:  We do our own
        scroll bar and the base class call is thus unnecessary.  It simply
        calls UpdateScrollBars(false).
    */
    virtual void UpdateScrollBars (void);
};

} } // end of namespace ::sd::slidesorter

#endif
