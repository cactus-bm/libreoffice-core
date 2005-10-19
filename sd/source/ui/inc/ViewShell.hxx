/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ViewShell.hxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: rt $ $Date: 2005-10-19 12:25:04 $
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

#ifndef SD_VIEW_SHELL_HXX
#define SD_VIEW_SHELL_HXX

#ifndef _SV_FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _SFXVIEWSH_HXX //autogen
#include <sfx2/viewsh.hxx>
#endif
#ifndef _SV_FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _SV_PRNTYPES_HXX //autogen
#include <vcl/prntypes.hxx>
#endif
#ifndef _TRANSFER_HXX //autogen
#include <svtools/transfer.hxx>
#endif
#include <comphelper/implementationreference.hxx>

#ifndef _SD_GLOB_HXX
#include "glob.hxx"
#endif
#ifndef _PRESENTATION_HXX
#include "pres.hxx"
#endif
#ifndef _SD_CFGID_HXX
#include "cfgids.hxx"
#endif
#ifndef _SD_VIEW_HXX
#include "View.hxx"
#endif

#ifndef INCLUDED_SDDLLAPI_H
#include "sddllapi.h"
#endif

#ifndef INCLUDED_MEMORY
#include <memory>
#define INCLUDED_MEMORY
#endif

class SdPage;
class SvxRuler;
class SdrOle2Obj;       // fuer die, die Teile von SVDRAW rausdefiniert haben
class ScrollBarBox;
class SdDrawDocument;
class ScrollBar;
class FmFormShell;
class SdOptionsPrintItem;
class MultiSelection;

extern String aEmptyStr;


namespace sd {

class Client;
class DrawDocShell;
class DrawController;
class FrameView;
class FuPoor;
class FuSearch;
class Slideshow;
class LayerTabBar;
class ObjectBarManager;
class View;
class ViewShellBase;
class ViewTabBar;
class Window;
class WindowUpdater;
class ZoomList;

#undef OUTPUT_DRAWMODE_COLOR
#undef OUTPUT_DRAWMODE_CONTRAST

/** Base class of the stacked shell hierarchy.

    <p>Despite its name this class is not a descendant of SfxViewShell
    but of SfxShell.  Its name expresses the fact that it acts like a
    view shell.  Beeing a stacked shell rather then being an actual view shell
    there can be several instances of this class that
    <ul>
    <li>all are based on the same view shell and thus show the same
    document and share common view functionality and</li>
    <li>are all visible at the same time and live in the same
    frame.</li>
    <ul></p>

    <p>This class replaces the former ViewShell class.</p>
*/
class ViewShell
    : public SfxShell
{
public:
    enum ShellType {
        ST_NONE,
        ST_DRAW,         // The Draw application.
        ST_IMPRESS,      // Main view of the Impress application.
        ST_NOTES,
        ST_HANDOUT,
        ST_OUTLINE,
        ST_SLIDE,         // Old slide view shell
        ST_SLIDE_SORTER,  // New Slide sorter.
        ST_PRESENTATION,
        ST_TASK_PANE
    };
    static const int MAX_HSPLIT_CNT = 1;
    static const int MAX_VSPLIT_CNT = 1;
    static const int MIN_SCROLLBAR_SIZE = 50;

    static const ULONG OUTPUT_DRAWMODE_COLOR = DRAWMODE_DEFAULT;
    static const ULONG OUTPUT_DRAWMODE_GRAYSCALE
        = DRAWMODE_GRAYLINE | DRAWMODE_GRAYFILL
        | DRAWMODE_BLACKTEXT | DRAWMODE_GRAYBITMAP
        | DRAWMODE_GRAYGRADIENT;
    static const int  OUTPUT_DRAWMODE_BLACKWHITE
        = DRAWMODE_BLACKLINE | DRAWMODE_BLACKTEXT
        | DRAWMODE_WHITEFILL | DRAWMODE_GRAYBITMAP
        | DRAWMODE_WHITEGRADIENT;
    static const int OUTPUT_DRAWMODE_CONTRAST
        = DRAWMODE_SETTINGSLINE | DRAWMODE_SETTINGSFILL
        | DRAWMODE_SETTINGSTEXT | DRAWMODE_SETTINGSGRADIENT;

    TYPEINFO();

    ViewShell (
        SfxViewFrame *pFrame,
        ::Window* pParentWindow,
        ViewShellBase& rViewShellBase,
        bool bAllowCenter = true);
    ViewShell (
        SfxViewFrame *pFrame,
        ::Window* pParentWindow,
        const ViewShell& rShell);
    virtual ~ViewShell (void);

    /** The Init method has to be called from the outside directly
        after a new object of this class has been created.  It can be
        used for that part of the initialisation that can be run only
        after the creation of the new object is finished.  This
        includes registration as listener at event broadcasters.

        Derived classes should call this method at the end of their
        Init() methods.
    */
    virtual void Init (void);

    /** The Exit() method has to be called before the destructor so that the
        view shell is still a valid object and can safely call methods that
        rely on that.
    */
    virtual void Exit (void);

    void Cancel();
    void CancelSearching();

    /** Return the window that is the parent of all controls of this view
        shell.  This may or may not be the window of the frame.
    */
    inline ::Window* GetParentWindow (void) const;

    inline ::sd::View* GetView (void) const;
    inline SdrView* GetDrawView (void) const;
    SD_DLLPUBLIC DrawDocShell* GetDocSh (void) const;

    SdDrawDocument*  GetDoc (void) const;

    SD_DLLPUBLIC SfxViewFrame* GetViewFrame (void) const;

    /** The active window is usually the mpContentWindow.  When there is a
        show running then the active window is a ShowWindow.
    */
    ::sd::Window* GetActiveWindow (void) const;

    /** Set the active window.  When the shell is displayed in the center
        pane then the window of the ViewShellBase is also set to the given
        window.
    */
    void SetActiveWindow (::sd::Window* pWindow);

    /** Return the rectangle that encloses all windows of the view.  That
        excludes the controls in the frame like rulers, scroll bars, tab
        bar, and buttons.
        @return
            The rectangle is returned in screen coordinates, i.e. pixel
            values relative to the upper left corner of the screen?.
    */
    const Rectangle& GetAllWindowRect (void);

    // Mouse- & Key-Events
    virtual void Paint (const Rectangle& rRect, ::sd::Window* pWin);
    virtual BOOL KeyInput(const KeyEvent& rKEvt, ::sd::Window* pWin);
    virtual void MouseMove(const MouseEvent& rMEvt, ::sd::Window* pWin);
    virtual void MouseButtonUp(const MouseEvent& rMEvt, ::sd::Window* pWin);
    virtual void MouseButtonDown(const MouseEvent& rMEvt, ::sd::Window* pWin);
    virtual void Command(const CommandEvent& rCEvt, ::sd::Window* pWin);
    virtual BOOL RequestHelp( const HelpEvent& rEvt, ::sd::Window* pWin );
    virtual long Notify( NotifyEvent& rNEvt, ::sd::Window* pWin );

    BOOL HandleScrollCommand(const CommandEvent& rCEvt, ::sd::Window* pWin);

    virtual void Draw(OutputDevice &rDev, const Region &rReg);

    virtual void SetUIUnit(FieldUnit eUnit);
    virtual void SetDefTabHRuler( UINT16 nDefTab );

    BOOL HasRuler (void);
    void SetRuler(BOOL bRuler);

    /** Set internal values of all scroll bars that determine thumb size and
        position.  The external values like size and position of the scroll
        bar controls are not modified.
    */
    virtual void UpdateScrollBars (void);
    void    Scroll(long nX, long nY);
    void    ScrollLines(long nX, long nY);
    virtual void    SetZoom(long nZoom);
    virtual void    SetZoomRect(const Rectangle& rZoomRect);
    void    InitWindows(const Point& rViewOrigin, const Size& rViewSize,
                        const Point& rWinPos, BOOL bUpdate = FALSE);
    void    InvalidateWindows();
    void    UpdateWindows();
    /** This method is still used by the OutlineViewShell to update the
        model according to the content of the outline view.  This in turn
        updates the previews in the slide sorter.
    */
     virtual void UpdatePreview (SdPage* pPage, BOOL bInit = FALSE);

    ObjectBarManager& GetObjectBarManager (void) const;

    /** Get a list of sub shells by appending them to the end of the given
        list of shells.  This is restricted to the object bars that are
        placed BELOW the called shell.  This method is typically called to
        gather all shells that are to be taken from or pushed on the stack
        of sub-shells.
    */
    virtual void GetLowerShellList (
        ::std::vector<SfxShell*>& rShellList) const;

    /** Get a list of sub shells by appending them to the end of the given
        list of shells.  This is restricted to the object bars that are
        placed ABOVE the called shell.  This method is typically called to
        gather all shells that are to be taken from or pushed on the stack
        of sub-shells.
    */
    virtual void GetUpperShellList (
        ::std::vector<SfxShell*>& rShellList) const;

    void    DrawMarkRect(const Rectangle& rRect) const;
    void    DrawFilledRect( const Rectangle& rRect, const Color& rLColor,
                            const Color& rFColor ) const;

    void    ExecReq( SfxRequest &rReq );

    ZoomList* GetZoomList (void);

    FrameView* GetFrameView (void);
    /** Setting a frame view triggers ReadFrameViewData() for the new
        frame.
        @param pFrameView
            The new frame view that replaces the old one.
    */
    void SetFrameView (FrameView* pFrameView);
    virtual void  ReadFrameViewData(FrameView* pView);
    virtual void  WriteFrameViewData();
    virtual void  WriteUserData(String& rString);
    virtual void  ReadUserData(const String& rString);

    virtual BOOL  ActivateObject(SdrOle2Obj* pObj, long nVerb);

    /** @returns
            current or selected page or 0. This method
            will fail in master page mode.

        @deprecated, please use getCurrentPage();
    */
    virtual SdPage* GetActualPage() = 0;

    /** @returns
            current or selected page or 0.
    */
    virtual SdPage* getCurrentPage() const = 0;

    FuPoor* GetOldFunction() const    { return pFuOld; }
    FuPoor* GetActualFunction() const { return pFuActual; }
    void SetCurrentFunction (FuPoor* pFunction);
    void SetOldFunction (FuPoor* pFunction);

    Slideshow* GetSlideShow() const { return mpSlideShow; }
    void SetSlideShow(Slideshow* pSlideShow );

    void    SetPageSizeAndBorder(PageKind ePageKind, const Size& rNewSize,
                            long nLeft, long nRight, long nUpper, long nLower,
                            BOOL bScaleAll, Orientation eOrient, USHORT nPaperBin,
                            BOOL bBackgroundFullSize );

    void    SetStartShowWithDialog( BOOL bIn = TRUE ) { bStartShowWithDialog = bIn; }
    BOOL    IsStartShowWithDialog() const { return bStartShowWithDialog; }

    USHORT  GetPrintedHandoutPageNum() const { return nPrintedHandoutPageNum; }

    virtual USHORT PrepareClose( BOOL bUI = TRUE, BOOL bForBrowsing = FALSE );

    void GetMenuState(SfxItemSet& rSet);

    virtual sal_Int8 AcceptDrop( const AcceptDropEvent& rEvt, DropTargetHelper& rTargetHelper,
                                 ::sd::Window* pTargetWindow, USHORT nPage, USHORT nLayer );
    virtual sal_Int8 ExecuteDrop( const ExecuteDropEvent& rEvt, DropTargetHelper& rTargetHelper,
                                  ::sd::Window* pTargetWindow, USHORT nPage, USHORT nLayer );

    virtual void WriteUserDataSequence ( ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >&, sal_Bool bBrowse = sal_False );
    virtual void ReadUserDataSequence ( const ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >&, sal_Bool bBrowse = sal_False );

    /** this method is called when the visible area of the view from this viewshell is changed */
    virtual void VisAreaChanged(const Rectangle& rRect);

    /** Create an accessible object representing the specified window.
        Overload this method to provide view mode specific objects.  The
        default implementation returns an empty reference.
        @param pWindow
            Make the document displayed in this window accessible.
        @return
            This default implementation returns an empty reference.
    */
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible>
        CreateAccessibleDocumentView (::sd::Window* pWindow);

    void SetWinViewPos(const Point& rWinPos, bool bUpdate);
    Point GetWinViewPos() const;
    Point GetViewOrigin() const;

    /** Return the window updater of this view shell.
        @return
            In rare circumstances the returned pointer may be <null/>,
            i.e. when no memory is available anymore.
    */
    ::sd::WindowUpdater* GetWindowUpdater (void) const;

    /** Return the border that is drawn arround the actual document view.
        The border contains typically rulers and scroll bars.
        @param bOuterResize
            When this flag is <TRUE/> then the border is used for an
            OuterResizePixel(), i.e. there is a given window size and the
            border elements are placed inside so that the document view has
            the given window size minus the border.
            When the flag is <FALSE/> then the border is used for an
            InnerResizePixel(), i.e. the document view has a given size and
            the border is placed outside.  In this scenario the parent
            window has the size of the document view plus the border.
    */
    SvBorder GetBorder (bool bOuterResize);

    /** Place all UI elements that are controlled by the view shell inside
        the specified rectangle.  This includes the border elements like
        rulers and scroll bars as well as the document view.  It is the
        responsibility of the caller to give an appropriate rectangle for an
        Inner- or OuterResizePixel().
        The rectangle is given in pixel coordinates relative to the parent
        window.
    */
    virtual void Resize (const Point &rPos, const Size &rSize);

    /** Set the position and size of the area which contains the GUI
        elements like rulers, sliders, and buttons as well as the document
        view.  Both size and position are expected to be in pixel
        coordinates.  The positions and sizes of the mentioned GUI elements
        are updated as well.

        <p> This method is implemented by first setting copying the given
        values to internal variables and then calling the
        <type>ArrangeGUIElements</type> method which performs the actual
        work of sizeing and arranging the UI elements accordingly.</p>
        @param rPos
            The position of the enclosing window relative to the document
            window.  This is only interesting if a Draw/Impress document
            view is embedded as OLE object into another document view.  For
            normal documents this position is (0,0).
        @param rSize
            The new size in pixel.
    */
    // This is to be replaced by Resize.
    //  virtual void AdjustPosSizePixel(const Point &rPos, const Size &rSize);

    /** Set position and size of the GUI elements that are controllerd by
        the view shell like rulers and scroll bars as well as the actual
        document view according to the position and size that were given
        with the last Resize() call.
    */
    virtual void ArrangeGUIElements (void);

    //  virtual void OuterResizePixel(const Point &rPos, const Size &rSize);
    //  virtual void InnerResizePixel(const Point &rPos, const Size &rSize);

    ViewShellBase& GetViewShellBase (void) const;

    /** Return <TRUE/> when the called view shell is the main sub shell of
        its ViewShellBase object, i.e. is display in the center pane.  This
        convenience function is equivalent to comparing the this pointer to
        the result of ViewShellBase::GetViewShell(PT_CENTER).
    */
    bool IsMainViewShell (void) const;

    /** Set or reset the flag that indicates whether the called shell is the
        one displayed in the center pane.  By default this flag is set to
        <FALSE/>.  For the main view shell it thus has to be set to <TRUE/>.
    */
    void SetIsMainViewShell (bool bIsMainViewShell);

    /** Return an object that implements the necessary UNO interfaces to act
        as a controller for the ViewShellBase object.  The controller is
        created if it does not exist.  It is owned by the caller.
        @return
            Returns NULL when the controller does not yet exist and can not
            be created.
    */
    virtual DrawController* GetController (void);

    /** Return the type of the shell.
    */
    virtual ShellType GetShellType (void) const;

    /** This method is more or less an alias to Deactivate().  It is called
        before an object of this class is taken from the stack of view
        shells.

        <p>When this method is not called before a view shell is taken from
        a stack then the Deactivate() call from the SFX as a response to
        RemoveSubShell() comes to late when the view shell is not on the
        stack anymore.</p>
    */
    virtual void Shutdown (void);

    /** This function is called from the underlying ViewShellBase
        object to handle a verb execution request.
    */
    virtual ErrCode DoVerb (long nVerb);

    void PrintOutline (
        SfxPrinter& rPrinter,
        SfxProgress& rProgress,
        const MultiSelection& rSelPages,
        const String& rTimeDateStr,
        const Font& rTimeDateFont,
        const SdOptionsPrintItem* pPrintOpts,
        USHORT nPage,
        USHORT nPageMax,
        USHORT nCopies,
        USHORT nProgressOffset,
        USHORT nTotal );
    void PrintHandout (
        SfxPrinter& rPrinter,
        SfxProgress& rProgress,
        const MultiSelection& rSelPages,
        const String& rTimeDateStr,
        const Font& rTimeDateFont,
        const SdOptionsPrintItem* pPrintOpts,
        USHORT nPage,
        USHORT nPageMax,
        USHORT nCopies,
        USHORT nProgressOffset,
        USHORT nTotal);
    void PrintStdOrNotes (
        SfxPrinter& rPrinter,
        SfxProgress& rProgress,
        const MultiSelection& rSelPages,
        const String& rTimeDateStr,
        const Font& rTimeDateFont,
        const SdOptionsPrintItem* pPrintOpts,
        USHORT nPage,
        USHORT nPageMax,
        USHORT nCopies,
        USHORT nProgressOffset,
        USHORT nTotal,
        PageKind ePageKind,
        BOOL bPrintMarkedOnly);

    virtual void UIActivating( SfxInPlaceClient* );
    virtual void UIDeactivated( SfxInPlaceClient* );

    /** Show controls of the UI or hide them, depending on the given flag.
        As a result the border is adapted.
    */
    virtual void ShowUIControls (bool bVisible = true);
    BOOL IsPageFlipMode(void) const;

protected:
    friend class ViewShellBase;

    /** Window inside the rulers and scroll bars that shows a view of the
        document.
    */

    ::std::auto_ptr< ::sd::Window> mpContentWindow;

    /// Horizontal scroll bar for the current slide is displayed when needed.
    ::std::auto_ptr<ScrollBar> mpHorizontalScrollBar;
    /// Vertical scroll bar for whole document is always visible.
    ::std::auto_ptr<ScrollBar> mpVerticalScrollBar;
    /// Horizontal ruler is not shown by default.
    ::std::auto_ptr<SvxRuler> mpHorizontalRuler;
    /// Vertical ruler is not shown by default.
    ::std::auto_ptr<SvxRuler> mpVerticalRuler;
    /// Filler of the little square enclosed by the two scroll bars.
    ::std::auto_ptr<ScrollBarBox> mpScrollBarBox;
    /// Layer tab bar.
    ::std::auto_ptr<LayerTabBar> mpLayerTabBar;

    /// This flag controls whether the rulers are visible.
    bool mbHasRulers;

    /// The active window.
    ::sd::Window* mpActiveWindow;
    ::sd::View* mpView;
    FrameView*  pFrameView;

    FuPoor*      pFuActual;
    FuPoor*      pFuOld;
    FuSearch*    pFuSearch;
    Slideshow*  mpSlideShow;
    ZoomList*    pZoomList;

    Point       aViewPos;
    Size        aViewSize;
    Size        aScrBarWH;

    BOOL        bCenterAllowed;           // wird an Fenster weitergegeben

    BOOL        bStartShowWithDialog;   // Praesentation wurde ueber Dialog gestartet
    USHORT      nPrintedHandoutPageNum; // Seitennummer der zu durckenden Handzettelseite

    //af    BOOL        bPrintDirectSelected;       // Print only selected objects in direct print
    //afString      sPageRange;                 // pagerange if selected objects in direct print

    /** Area covered by all windows, i.e. the area of the parent window
        without the controls at the borders like rulers, scroll bars, tab
        bar, buttons.
        This rectangle may be set in window coordinates (i.e. pixel values
        relative to the parent window).  It is transformed by every call to
        GetAllWindowRectangle() into screen coordinates (relative to the
        upper left corner of the screen.
    */
    Rectangle maAllWindowRectangle;

    /// The type of the shell.  Returned by GetShellType().
    ShellType meShellType;

    /** Main controller of the view shell.  During the switching from one
        stacked shell to another this pointer may be NULL.
    */
    ::comphelper::ImplementationReference<
        DrawController,
        ::com::sun::star::uno::XInterface,
        ::com::sun::star::uno::XWeak>
            mpController;

    class Implementation;
    ::std::auto_ptr<Implementation> mpImpl;

    // #96090# Support methods for centralized UNDO/REDO
    virtual SfxUndoManager* ImpGetUndoManager (void) const;
    void ImpGetUndoStrings(SfxItemSet &rSet) const;
    void ImpGetRedoStrings(SfxItemSet &rSet) const;
    void ImpSidUndo(BOOL bDrawViewShell, SfxRequest& rReq);
    void ImpSidRedo(BOOL bDrawViewShell, SfxRequest& rReq);

    DECL_LINK( HScrollHdl, ScrollBar * );
    DECL_LINK( VScrollHdl, ScrollBar * );

    // virt. Scroll-Handler, hier koennen sich abgeleitete Klassen einklinken
    virtual long VirtHScrollHdl(ScrollBar* pHScroll);
    virtual long VirtVScrollHdl(ScrollBar* pVScroll);

    // virtuelle Funktionen fuer Lineal-Handling
    virtual SvxRuler* CreateHRuler(::sd::Window* pWin, BOOL bIsFirst) { return NULL; }
    virtual SvxRuler* CreateVRuler(::sd::Window* pWin) { return NULL; }
    virtual void UpdateHRuler() {}
    virtual void UpdateVRuler() {}

    // Zeiger auf ein zusaetzliches Control im horizontalen ScrollBar
    // abgeleiteter Klassen (z.B. ein TabBar) zurueckgeben
    virtual long GetHCtrlWidth() { return 0; }

    virtual void Activate(BOOL IsMDIActivate);
    virtual void Deactivate(BOOL IsMDIActivate);

    virtual void SetZoomFactor( const Fraction &rZoomX,
                                const Fraction &rZoomY );


    //  virtual void PreparePrint(PrintDialog* pPrintDialog = 0);


    void PrintPage( SfxPrinter& rPrinter, ::sd::View* pPrintView,
                    SdPage* pPage, BOOL bPrintMarkedOnly );

    /** Depending on the given request create a new page or duplicate an
        existing one.  A new page is created behind the given slide.
        @param rRequest
            The request as passed to an Execute() method.  Its arguments are
            evaluated.  Its slot id determines whether to create or
            duplicate a slide.
        @param pPage
            This page is either duplicated or becomes the predecessor of the
            new slide.  If NULL a duplication request is ignored.  A new
            slide is inserted as first slide.
        @return
            The new slide is returned.  If for some reason a new page can
            not be created then NULL is returned.
    */
    virtual SdPage* CreateOrDuplicatePage (
        SfxRequest& rRequest,
        PageKind ePageKind,
        SdPage* pPage);

private:
    ::Window* mpParentWindow;
    ::std::auto_ptr<ObjectBarManager> mpObjectBarManager;
    /** This window updater is used to keep all relevant windows up to date
        with reference to the digit langugage used to display digits in text
        shapes.
    */
    ::std::auto_ptr< ::sd::WindowUpdater> mpWindowUpdater;

    /** Code common to all constructors.  It generally is a bad idea
        to call this function from outside a constructor.
    */
    void Construct (void);

    DECL_LINK(FrameWindowEventListener, VclSimpleEvent*);

    /** Create the rulers.
    */
    void SetupRulers (void);
};




::Window* ViewShell::GetParentWindow (void) const
{
    return mpParentWindow;
}

::sd::View* ViewShell::GetView (void) const
{
    return mpView;
}

SdrView* ViewShell::GetDrawView (void) const
{
    return static_cast<SdrView*>(mpView);
}

} // end of namespace sd

#endif
