/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SlsViewOverlay.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: vg $ $Date: 2006-04-06 16:25:38 $
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

#ifndef SD_SLIDESORTER_VIEW_OVERLAY_HXX
#define SD_SLIDESORTER_VIEW_OVERLAY_HXX

#include "model/SlsSharedPageDescriptor.hxx"

#include <osl/mutex.hxx>
#include <tools/gen.hxx>
#include <vector>
#include <boost/weak_ptr.hpp>

class OutputDevice;
class Region;

namespace sd { namespace slidesorter {
class SlideSorterViewShell;
} }

namespace sd { namespace slidesorter { namespace model {
class PageEnumeration;
} } }

namespace sd { namespace slidesorter { namespace controller {
class SlideSorterController;
} } }

namespace sd { namespace slidesorter { namespace view {


class ViewOverlay;
class SelectionRectangleOverlay;
class InsertionIndicatorOverlay;
class SubstitutionOverlay;

/** This base class of overlay graphics keeps track of the visibility of
    graphical objects that possibly are drawn in XOR paint mode.  This makes
    it possibly to switch such an overlay on or off without knowing whether
    it is visible.
*/
class OverlayBase
{
public:
    OverlayBase (ViewOverlay& rViewOverlay);
    virtual ~OverlayBase (void);

    virtual void Paint (void);

    virtual void Show (void);
    virtual void Hide (void);
    void Toggle (void);
    bool IsShowing (void);
    ViewOverlay& GetViewOverlay (void);

protected:
    ::osl::Mutex maMutex;

    ViewOverlay& mrViewOverlay;

    bool mbIsShowing;
};




/** This class manages the substitution display of the page objects.  This
    subsitution is used to visualize the selected page objects during a
    mouse drag operation.
*/
class SubstitutionOverlay
    : public OverlayBase
{
public:
    SubstitutionOverlay (ViewOverlay& rViewOverlay);
    virtual ~SubstitutionOverlay (void);

    virtual void Paint (void);

    /** Setup the substitution display of the given set of selected pages.
        The given mouse position is remembered so that it later can be
        returned by GetPosition(). This is a convenience feature.
    */
    void Create (
        model::PageEnumeration& rSelection,
        const Point& rPosition);

    /** Clear the substitution display.  Until the next call of Create() no
        substution is painted.
    */
    void Clear (void);

    /** Move the substitution display by the given amount of pixels.
    */
    void Move (const Point& rOffset);
    void SetPosition (const Point& rPosition);
    const Point& GetPosition (void) const;

private:
    /// List of page object substitution displays.
    typedef ::std::vector<Rectangle> SubstitutionShapeList;
    SubstitutionShapeList maShapes;
    Point maPosition;
};




class SelectionRectangleOverlay
    : public OverlayBase
{
public:
    SelectionRectangleOverlay (ViewOverlay& rViewOverlay);

    virtual void Paint (void);

    virtual void Show (void);
    virtual void Hide (void);

    void Start (const Point& rAnchor);
    void Update (const Point& rSecondCorner);

    Rectangle GetSelectionRectangle (void);

private:
    Point maAnchor;
    Point maSecondCorner;
};




/** The insertion indicator is painted as a vertical or horizonal bar
    in the space between slides.
*/
class InsertionIndicatorOverlay
    : public OverlayBase
{
public:
    InsertionIndicatorOverlay (ViewOverlay& rViewOverlay);

    void SetPositionAndSize (const Rectangle& rBoundingBox);

    virtual void Paint (void);

    /** Given a position in model coordinates this method calculates the
        insertion marker both as an index in the document and as a rectangle
        used for drawing the insertion indicator.
    */
    void SetPosition (const Point& rPosition);

    sal_Int32 GetInsertionPageIndex (void) const;

private:
    Rectangle maBoundingBox;
    sal_Int32 mnInsertionIndex;
};




/** Paint a frame around the slide preview under the mouse.  The actual
    painting is done by the PageObjectViewObjectContact of the slidesorter.
    This class is responsible for the coordination of the right time for the
    painting.
*/
class MouseOverIndicatorOverlay
    : public OverlayBase
{
public:
    MouseOverIndicatorOverlay (ViewOverlay& rViewOverlay);

    /** Set the page object for which to paint a mouse over indicator.
        @param pContact
            A value of <NULL/> indicates to not paint the mouse over indicator.
    */
    void SetSlideUnderMouse (const model::SharedPageDescriptor& rpDescriptor);

    virtual void Paint (void);

private:
    /** The page under the mouse is stored as weak shared pointer so that
        model changes can be handled without having the SlideSorterModel
        inform this class explicitly.
    */
    ::boost::weak_ptr<model::PageDescriptor> mpPageUnderMouse;
};




/** The view overlay manages and paints some indicators that are painted on
    top of the regular view content (the page objects).  It is separated
    from the view to allow the indicators to be altered in position and size
    without to repaint the whole view content (inside that the bounding box
    of the indicator).  One technique to achive this is to use XOR-painting.

    The view overlay itself simply provides the more specialized classes
    that handle individual indicators.

*/
class ViewOverlay
{
public:
    ViewOverlay (SlideSorterViewShell& rViewShell);
    ~ViewOverlay (void);

    SelectionRectangleOverlay& GetSelectionRectangleOverlay (void);
    MouseOverIndicatorOverlay& GetMouseOverIndicatorOverlay (void);
    InsertionIndicatorOverlay& GetInsertionIndicatorOverlay (void);
    SubstitutionOverlay& GetSubstitutionOverlay (void);

    void Paint (void);

    /** The overlay paint type describes how an overlay is painted.  That can be
        either by using XOR operation or by doing a regular paint.
    */
    enum OverlayPaintType { OPT_ALL, OPT_XOR, OPT_PAINT };

    /** As a preparation for draw operations that are not caused by the
        overlays this method saves the current state of all overlays so that
        the next call to Restore() can restore them.  After that it hides
        the overlays so they do not interfere with the drawing operations.
        @param eType
            This parameter specifies what overlays to hide.
    */
    void HideAndSave (OverlayPaintType eType = OPT_ALL);

    /** Restore the state of the overlays that has been saved in an earlier
        call of HideAndSave().
    */
    void Restore (void);

    controller::SlideSorterController& GetController (void);
    SlideSorterViewShell& GetViewShell (void);

private:
    SlideSorterViewShell& mrViewShell;
    SelectionRectangleOverlay maSelectionRectangleOverlay;
    MouseOverIndicatorOverlay maMouseOverIndicatorOverlay;
    InsertionIndicatorOverlay maInsertionIndicatorOverlay;
    SubstitutionOverlay maSubstitutionOverlay;

    OverlayPaintType meSavedStateType;

    bool mbSelectionRectangleWasVisible;
    bool mbMouseOverIndicatorWasVisible;
    bool mbInsertionIndicatorWasVisible;
    bool mbSubstitutionDisplayWasVisible;

    /** The number HideAndSave() has been called more than Restore(). Only
        when the value is 1 does Restore() really restore the overlays.
    */
    int mnHideAndSaveLevel;
};



} } } // end of namespace ::sd::slidesorter::view

#endif
