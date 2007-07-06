/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ViewShellBase.hxx,v $
 *
 *  $Revision: 1.20 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-06 13:13:26 $
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

#ifndef SD_VIEW_SHELL_BASE_HXX
#define SD_VIEW_SHELL_BASE_HXX

#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif

#ifndef SD_GLOB_HXX
#include "glob.hxx"
#endif
#ifndef _SFXVIEWSH_HXX
#include <sfx2/viewsh.hxx>
#endif
#ifndef _VIEWFAC_HXX
#include <sfx2/viewfac.hxx>
#endif
#include <memory>
#include <boost/shared_ptr.hpp>

#include <set>

class SdDrawDocument;
class SfxRequest;

namespace sd { namespace tools {
class EventMultiplexer;
} }

namespace sd {

class DrawController;
class DrawDocShell;
class FormShellManager;
class PrintManager;
class ToolBarManager;
class UpdateLockManager;
class ViewShell;
class ViewShellManager;
class CustomHandleManager;

/** SfxViewShell descendant that the stacked Draw/Impress shells are
    based on.

    <p>The "base" part of the name does not mean that this is a base
    class of some class hierarchy.  It rather is the base of the
    stacked shells.</p>

    <p>This class starts as a new and relatively small class.  Over
    time as much code as possible should be moved from the stacked
    shells to this class.</p>
*/
class ViewShellBase
    : public SfxViewShell
{
public:
    TYPEINFO();
    SFX_DECL_VIEWFACTORY(ViewShellBase);
    SFX_DECL_INTERFACE(SD_IF_SDVIEWSHELLBASE)

    /** This constructor is used by the view factory of the SFX macros.
        Note that LateInit() has to be called after the constructor
        terminates and before doing anything else.
    */
    ViewShellBase (
        SfxViewFrame *pFrame,
        SfxViewShell* pOldShell);

    virtual ~ViewShellBase (void);

    /** This method is part of the object construction.  It HAS to be called
        after the constructor has created a new object.
    */
    virtual void LateInit (const ::rtl::OUString& rsDefaultView);

    ViewShellManager& GetViewShellManager (void) const;

    /** Return the main view shell stacked on the called ViewShellBase
        object.  This is usually the view shell displayed in the center
        pane.
    */
    ::boost::shared_ptr<ViewShell> GetMainViewShell (void) const;

    /** When given a view frame this static method returns the
        corresponding sd::ViewShellBase object.
        @return
            When the SfxViewShell of the given frame is not a
            ViewShellBase object then NULL is returned.
    */
    static ViewShellBase* GetViewShellBase (SfxViewFrame* pFrame);

    DrawDocShell* GetDocShell (void) const;
    SdDrawDocument* GetDocument (void) const;

    /** Callback function for retrieving item values related to menu entries.
    */
    void GetMenuState (SfxItemSet& rSet);

    /** Callback function for general slot calls.  At the moment these are
        slots for switching the pane docking windows on and off.
    */
    virtual void Execute (SfxRequest& rRequest);

    /** Callback function for retrieving item values related to certain
        slots.  This is the companion of Execute() and handles the slots
        concerned with showing the pane docking windows.
    */
    virtual void GetState (SfxItemSet& rSet);

    SvBorder GetBorder (bool bOuterResize);
    virtual void InnerResizePixel (const Point& rOrigin, const Size& rSize);
    virtual void OuterResizePixel (const Point& rOrigin, const Size& rSize);

    /** This call is forwarded to the main sub-shell.
    */
    virtual ErrCode DoVerb (long nVerb);

    /// Forwarded to the print manager.
    virtual SfxPrinter* GetPrinter (BOOL bCreate = FALSE);

    /// Forwarded to the print manager.
    virtual USHORT SetPrinter (
        SfxPrinter* pNewPrinter,
        USHORT nDiffFlags = SFX_PRINTER_ALL);

    /// Forwarded to the print manager.
    virtual PrintDialog* CreatePrintDialog (::Window *pParent);

    /// Forwarded to the print manager.
    virtual SfxTabPage* CreatePrintOptionsPage (
        ::Window *pParent,
        const SfxItemSet &rOptions);

    /// Forwarded to the print manager.
    virtual USHORT Print (SfxProgress& rProgress, BOOL bIsAPI, PrintDialog* pDialog);

    /// Forwarded to the print manager.
    virtual ErrCode DoPrint (
        SfxPrinter *pPrinter,
        PrintDialog *pPrintDialog,
        BOOL bSilent, BOOL bIsAPI );

    /// Forwarded to the print manager.
    USHORT SetPrinterOptDlg (
        SfxPrinter* pNewPrinter,
        USHORT nDiffFlags = SFX_PRINTER_ALL,
        BOOL _bShowDialog = TRUE);

    virtual void PreparePrint (PrintDialog* pPrintDialog);

    /// Forward methods to main sub shell.
    virtual void WriteUserDataSequence (
        ::com::sun::star::uno::Sequence <
        ::com::sun::star::beans::PropertyValue >&,
        sal_Bool bBrowse = sal_False);

    /** Pass the given properties to the main view shell.  After that we
        ensure that the right view shell type is displayed in the center
        pane.
    */
    virtual void ReadUserDataSequence (
        const ::com::sun::star::uno::Sequence <
        ::com::sun::star::beans::PropertyValue >&,
        sal_Bool bBrowse = sal_False);

    virtual void UIActivating( SfxInPlaceClient* );
    virtual void UIDeactivated( SfxInPlaceClient* );
    virtual void Activate (BOOL IsMDIActivate);
    virtual void Deactivate (BOOL IsMDIActivate);
    virtual void SetZoomFactor (
        const Fraction &rZoomX,
        const Fraction &rZoomY);
    virtual USHORT PrepareClose (BOOL bUI = TRUE, BOOL bForBrowsing = FALSE);
    virtual void WriteUserData (String&, BOOL bBrowse = FALSE);
    virtual void ReadUserData (const String&, BOOL bBrowse = FALSE);
    virtual SdrView* GetDrawView (void) const;
    virtual void AdjustPosSizePixel (const Point &rOfs, const Size &rSize);

    /** When <TRUE/> is given, then the mouse shape is set to hour glass (or
        whatever the busy shape looks like on the system.)
    */
    void SetBusyState (bool bBusy);

    /** Call this method when the controls of this view shell or the
        embedded sub shell need to be rearranged.  This is necessary
        e.g. when the border has been modified (UpdateBorder() calls this
        method).

        This method is like ResizePixel() with no arguments.
    */
    void Rearrange (void);

    /** Update the border that is set with SfxViewShell::SetBorderPixel().
        This is done by adding the border used by the ViewShellBase itself
        with the border used by the main view shell.

        @param bForce   if true the borders are also updated if old border
                        and new border are same.
    */
    void UpdateBorder ( bool bForce = false );

    /** With this method the UI controls can be turned on or off.  It is
        used by the FuSlideShow to hide the UI controls while showing a
        non-full-screen or in-window presentation in the center pane.
    */
    void ShowUIControls (bool bVisible);

    /** this method starts the presentation by
        executing the slot SID_PRESENTATION asynchronous */
    void StartPresentation();

    /** this methods ends the presentation by
        executing the slot SID_PRESENTATION_END asynchronous */
    void StopPresentation();

    /** Return an event multiplexer.  It is a single class that forwards
        events from various sources.  This method must not be called before
        LateInit() has terminated.
    */
    tools::EventMultiplexer& GetEventMultiplexer (void);

    /** returns the complete area of the current view relative to the frame
        window
    */
    const Rectangle& getClientRectangle() const;

    ::boost::shared_ptr<UpdateLockManager> GetUpdateLockManager (void) const;

    ::boost::shared_ptr<ToolBarManager> GetToolBarManager (void) const;

    FormShellManager& GetFormShellManager (void) const;

    DrawController& GetDrawController (void) const;

    void SetViewTabBar (const ::rtl::Reference<ViewTabBar>& rViewTabBar);

    /** Return the window that is used by the main view shell to display its
        view and other UI elements, like scroll bars and rulers.  Ownership
        of that window remains with the called ViewShellBase object.
    */
    ::Window* GetViewWindow (void);

    CustomHandleManager& getCustomHandleManager() const;

protected:
    osl::Mutex maMutex;

    virtual void SFX_NOTIFY(SfxBroadcaster& rBC,
        const TypeId& rBCType,
        const SfxHint& rHint,
        const TypeId& rHintType);

    virtual void InitializeFramework (void);

private:
    class Implementation;
    ::std::auto_ptr<Implementation> mpImpl;
    ::std::auto_ptr<ViewShellManager> mpViewShellManager;
    DrawDocShell* mpDocShell;
    SdDrawDocument* mpDocument;

    /// The print manager is responsible for printing documents.
    ::std::auto_ptr<PrintManager> mpPrintManager;

    ::std::auto_ptr<FormShellManager> mpFormShellManager;

    ::std::auto_ptr<tools::EventMultiplexer> mpEventMultiplexer;

    ::boost::shared_ptr<UpdateLockManager> mpUpdateLockManager;

    ::std::auto_ptr<CustomHandleManager> mpCustomHandleManager;

    /** Determine from the properties of the document shell the initial type
        of the view shell in the center pane.  We use this method to avoid
        starting with the wrong type.  When ReadUserDataSequence() is called
        we check that the right type is active and change again if that is
        not the case because something went wrong.
    */
    ::rtl::OUString GetInitialViewShellType (void);
};

class ICustomhandleSupplier
{
public:
    virtual void addCustomHandler( SdrView& rSourceView, ViewShell::ShellType eShellType, SdrHdlList& rHandlerList ) = 0;
};

class CustomHandleManager : public ICustomhandleSupplier
{
public:
    CustomHandleManager( ViewShellBase& rViewShellBase  );
    virtual ~CustomHandleManager();

    void registerSupplier( ICustomhandleSupplier* pSupplier );
    void unRegisterSupplier( ICustomhandleSupplier* pSupplier );

    virtual void addCustomHandler( SdrView& rSourceView, ViewShell::ShellType eShellType, SdrHdlList& rHandlerList );

private:
    ViewShellBase& mrViewShellBase;
    std::set< ICustomhandleSupplier* > maSupplier;
};

} // end of namespace sd

#endif
