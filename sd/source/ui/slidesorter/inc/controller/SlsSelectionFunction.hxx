/*************************************************************************
 *
 *  $RCSfile: SlsSelectionFunction.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-01-28 15:42:13 $
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

#ifndef SD_SLIDESORTER_SELECTION_FUNCTION_HXX
#define SD_SLIDESORTER_SELECTION_FUNCTION_HXX

#include "controller/SlsSlideFunction.hxx"

#ifndef _LIST_HXX
#include <tools/list.hxx>
#endif

class SdSlideViewShell;
class SdWindow;
class SdSlideView;
class SdDrawDocument;
class Sound;

namespace sd { namespace slidesorter { namespace model {
class PageDescriptor;
} } }

namespace sd { namespace slidesorter { namespace controller {

class SlideSorterController;

class SelectionFunction
    : public SlideFunction
{
public:
    TYPEINFO();

    SelectionFunction (
        SlideSorterController& rController,
        SfxRequest& rRequest);

    virtual ~SelectionFunction (void);
                                                   // Mouse- & Key-Events
    virtual BOOL KeyInput(const KeyEvent& rKEvt);
    virtual BOOL MouseMove(const MouseEvent& rMEvt);
    virtual BOOL MouseButtonUp(const MouseEvent& rMEvt);
    virtual BOOL MouseButtonDown(const MouseEvent& rMEvt);
    virtual void Paint(const Rectangle& rRect, SdWindow* pWin);

    virtual void Activate();           // Function aktivieren
    virtual void Deactivate();         // Function deaktivieren

    virtual void ScrollStart();
    virtual void ScrollEnd();

    /// Forward to the clipboard manager.
    virtual void DoCut (void);

    /// Forward to the clipboard manager.
    virtual void DoCopy (void);

    /// Forward to the clipboard manager.
    virtual void DoPaste (void);

    /** is called when the current function should be aborted. <p>
        This is used when a function gets a KEY_ESCAPE but can also
        be called directly.

        @returns
            true if a active function was aborted
    */
    virtual bool cancel();

protected:
    SlideSorterController& mrController;


private:
    /// Set in MouseButtonDown this flag indicates that a page has been hit.
    bool mbPageHit;

    /// This flag indicates whether the selection rectangle is visible.
    bool mbRectangleSelection;

    /// The rectangle of the mouse drag selection.
    Rectangle maDragSelectionRectangle;
    bool mbDragSelection;

    /// Box of the insert marker in model coordinates.
    Rectangle maInsertionMarkerBox;
    Sound* mpSound;
    class ShowingEffectInfo;
    ShowingEffectInfo* mpShowingEffectInfo;

    model::PageDescriptor* mpRangeSelectionAnchor;

    /** We use this flag to filter out the cases where MouseMotion() is called
        with a pressed mouse button but without a prior MouseButtonDown()
        call.  This is an indication that the mouse button was pressed over
        another control, e.g. the view tab bar, and that a re-layout of the
        controls moved the slide sorter under the mouse.
    */
    bool mbProcessingMouseButtonDown;

    /** Show the effect of the specified page.
    */
    void ShowEffect (model::PageDescriptor& rDescriptor);

    /** Return whether there is currently an effect being shown.
    */
    bool IsShowingEffect (void) const;

    DECL_LINK( DragSlideHdl, Timer* );
    void StartDrag (void);

    /** Set the selection to exactly the specified page and also set it as
        the current page.  Furthermore, if the view on which this selection
        function is working is the main view then the view is switched to
        the regular editing view.
        @param rDescriptor
            This page descriptor represents the page which will (a) replace
            the current selection and (b) become the current page of the
            main view.
    */
    void SetCurrentPageAndSwitchView (model::PageDescriptor& rDescriptor);

    /** Make the slide nOffset slides away of the current one the new
        current slide.  When the new index is outside the range of valid
        page numbers it is clipped to that range.
        @param nOffset
            When nOffset is negative then go back.  When nOffset if positive go
            forward.  When it is zero then ignore the call.
    */
    void GotoNextPage (int nOffset);

    void ProcessMouseEvent (sal_uInt32 nEventType, const MouseEvent& rEvent);

    // What follows are a couple of helper methods that are used by
    // ProcessMouseEvent().

    /// Select the specified page and set the selection anchor.
    void SelectHitPage (model::PageDescriptor& rDescriptor);
    /// Deselect the specified page.
    void DeselectHitPage (model::PageDescriptor& rDescriptor);
    /// Deselect all pages.
    void DeselectAllPages (void);

    /** for a possibly following mouse motion by starting the drag timer
        that after a short time of pressed but un-moved mouse starts a drag
        operation.
    */
    void PrepareMouseMotion (const Point& aMouseModelPosition);

    /** Set the current page of the main view to the one specified by the
        given descriptor.
    */
    void SetCurrentPage (model::PageDescriptor& rDescriptor);
    /** Select all pages between and including the selection anchor and the
        specified page.
    */
    void RangeSelect (model::PageDescriptor& rDescriptor);

    /** Start a rectangle selection at the given position.
    */
    void StartRectangleSelection (const Point& aMouseModelPosition);

    /** Update the rectangle selection so that the given position becomes
        the new second point of the selection rectangle.
    */
    void UpdateRectangleSelection (const Point& aMouseModelPosition);

    /** Select all pages that lie completly in the selection rectangle.
    */
    void ProcessRectangleSelection (bool bToggleSelection);


    /** Create a substitution display of the currently selected pages and
        use the given position as the anchor point.
    */
    void CreateSubstitution (const Point& rMouseModelPosition);

    /** Move the substitution display by the distance the mouse has
        travelled since the last call to this method or to
        CreateSubstitution().  The given point becomes the new anchor.
    */
    void UpdateSubstitution (const Point& rMouseModelPosition);

    /** Move the substitution display of the currently selected pages.
    */
    void MoveSubstitution (void);
};

} } } // end of namespace ::sd::slidesorter::controller

#endif

