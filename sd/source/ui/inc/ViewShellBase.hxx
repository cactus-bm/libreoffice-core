/*************************************************************************
 *
 *  $RCSfile: ViewShellBase.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2004-01-20 11:46:43 $
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

#ifndef SD_VIEW_SHELL_BASE_HXX
#define SD_VIEW_SHELL_BASE_HXX

#ifndef SD_GLOB_HXX
#include "glob.hxx"
#endif
#ifndef _SFXVIEWSH_HXX
#include <sfx2/viewsh.hxx>
#endif
#ifndef _VIEWFAC_HXX
#include <sfx2/viewfac.hxx>
#endif
#ifndef SD_SUB_SHELL_MANAGER_HXX
#include "SubShellManager.hxx"
#endif
#ifndef SD_PRINT_MANAGER_HXX
#include "PrintManager.hxx"
#endif
#include <memory>

class SdDrawDocument;
class SfxRequest;


namespace sd {

class DrawController;
class DrawDocShell;
class ObjectBarManager;
class ViewShell;

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
    SFX_DECL_INTERFACE(SD_IF_SDVIEWSHELLBASE);

    //    ViewShellBase (SfxViewFrame *pFrame, USHORT nFlags);
    /** This constructor is used by the view factory of the SFX
        macros.
    */
    ViewShellBase (SfxViewFrame *pFrame, SfxViewShell* pOldShell,
        ViewShell::ShellType eDefaultSubShell = ViewShell::ST_IMPRESS);
    virtual ~ViewShellBase (void);

    SubShellManager& GetSubShellManager (void) const;
    ObjectBarManager& GetObjectBarManager (void) const;

    /** When given a view frame this static method returns the
        corresponding sd::ViewShell object.  In the old Impress this
        is the single view shell associated with this frame.  In the
        new Impress this will be one that corresponds with the central
        pane.
    */
    static ViewShell* GetMainViewShell (SfxViewFrame* pFrame);

    DrawDocShell* GetDocShell (void) const;
    SdDrawDocument* GetDocument (void) const;

    /** Callback function for slots related to changing the view or
        edit mode.
    */
    void ExecuteModeChange (SfxRequest& rRequest);

    /** Callback function for retrieving item values related to menu entries.
    */
    void GetMenuState (SfxItemSet& rSet);

    /** Return the controller associated with the view shell.

        <p>The call can fail for three reasons:<br>
            1. The controller has been disposed but the pointer of the
            controller is returned (not NULL).  May be fixed by
            registering as dispose listener.  Usually the controller
            is disposed when the view shell is destroyed.<br>
            2. The controller is not the controller of the view
            shell.  This should not be possible because the controller
            is registered at the view shell during its construction
            and normally can not be replaced.</p>
    */
    DrawController* GetDrawController (void);

    virtual void InnerResizePixel (const Point &rPos, const Size &rSize);

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
    virtual USHORT Print (SfxProgress& rProgress, PrintDialog* pDialog);

    /// Forwarded to the print manager.
    virtual ErrCode DoPrint (
        SfxPrinter *pPrinter,
        PrintDialog *pPrintDialog,
        BOOL bSilent);

    /// Forwarded to the print manager.
    USHORT SetPrinterOptDlg (
        SfxPrinter* pNewPrinter,
        USHORT nDiffFlags = SFX_PRINTER_ALL,
        BOOL _bShowDialog = TRUE);

    void PreparePrint (PrintDialog* pPrintDialog);

protected:
    virtual void SFX_NOTIFY(SfxBroadcaster& rBC,
        const TypeId& rBCType,
        const SfxHint& rHint,
        const TypeId& rHintType);

private:
    osl::Mutex maMutex;

    ::std::auto_ptr<SubShellManager> mpSubShellManager;

    DrawDocShell* mpDocShell;
    SdDrawDocument* mpDocument;

    /** Main controller of the view shell.
    */
    DrawController* mpController;

    /// The print manager is responsible for printing documents.
    PrintManager maPrintManager;

    /** Code common to all constructors.
    */
    void Construct (ViewShell::ShellType eDefaultSubShell);
};

} // end of namespace sd

#endif
