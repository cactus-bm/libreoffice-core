/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SlsClipboard.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: hr $ $Date: 2005-09-23 11:29:58 $
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
#ifndef SD_SLIDESORTER_CLIPBOARD
#define SD_SLIDESORTER_CLIPBOARD

#include "ViewClipboard.hxx"

#include <sal/types.h>
#include <tools/solar.h>
#include <svx/svdpage.hxx>

#include <set>

class SfxRequest;
class Window;

struct AcceptDropEvent;
class DropTargetHelper;
struct ExecuteDropEvent;
class Point;
class SdPage;
class Window;

namespace sd {
class Window;
}

namespace sd { namespace slidesorter { namespace model {
class PageDescriptor;
} } }

namespace sd { namespace slidesorter { namespace controller {

class SlideSorterController;

class Clipboard
    : public ViewClipboard
{
public:
    Clipboard (SlideSorterController& rController);
    ~Clipboard (void);

    void HandleSlotCall (SfxRequest& rRequest);

    void DoCut (::Window* pWindow = 0);
    void DoCopy (::Window* pWindow = 0);
    void DoPaste (::Window* pWindow = 0);
    void DoDelete (::Window* pWindow = 0);

    void StartDrag (
        const Point& rDragPt,
        ::Window* pWindow );

    void DragFinished (
        sal_Int8 nDropAction);

    sal_Int8 AcceptDrop (
        const AcceptDropEvent& rEvt,
        DropTargetHelper& rTargetHelper,
        ::sd::Window* pTargetWindow = NULL,
        USHORT nPage = SDRPAGE_NOTFOUND,
        USHORT nLayer = SDRPAGE_NOTFOUND );

    sal_Int8 ExecuteDrop (
        const ExecuteDropEvent& rEvt,
        DropTargetHelper& rTargetHelper,
        ::sd::Window* pTargetWindow = NULL,
        USHORT nPage = SDRPAGE_NOTFOUND,
        USHORT nLayer = SDRPAGE_NOTFOUND);

protected:
    virtual USHORT DetermineInsertPosition (
        const SdTransferable& rTransferable);

    virtual USHORT InsertSlides (
        const SdTransferable& rTransferable,
        USHORT nInsertPosition);

private:
    SlideSorterController& mrController;

    typedef ::std::vector<SdPage*> PageList;
    /** Remember the pages that are dragged to another document or to
        another place in the same document so that they can be removed after
        a move operation.
    */
    PageList maPagesToRemove;

    /** Remember the pages inserted from another document or another place
        in the same document so that they can be selected after the
        drag-and-drop operation is completed.
    */
    PageList maPagesToSelect;

    /** When pages are moved or copied then the selection of the slide
        sorter has to be updated.  This flag is used to remember whether the
        selection has to be updated or can stay as it is (FALSE).
    */
    bool mbUpdateSelectionPending;

    void CreateSlideTransferable (
        ::Window* pWindow,
        bool bDrag);

    /** Select the pages stored in the maPagesToSelect member.  The list in
        the member is cleared afterwards.
    */
    void SelectPages (void);

    /** Determine the position of where to insert the pages in the current
        transferable of the sd module.
        @param pWindow
            This window is used as parent for dialogs that have to be shown
            to the user.
        @return
            The index in the range [0,n] (both inclusive) with n the number
            of pages is returned.
    */
    sal_Int32 GetInsertionPosition (::Window* pWindow);

    /** Paste the pages of the transferable of the sd module at the given
        position.
        @param nInsertPosition
            The position at which to insert the pages.  The valid range is
            [0,n] (both inclusive) with n the number of pages in the
            document.
        @return
            The number of inserted pages is returned.
    */
    sal_Int32 PasteTransferable (sal_Int32 nInsertPosition);

    /** Select a range of pages of the model.  Typicall usage is the
        selection of newly inserted pages.
        @param nFirstIndex
            The index of the first page to select.
        @param nPageCount
            The number of pages to select.
    */
    void SelectPageRange (sal_Int32 nFirstIndex, sal_Int32 nPageCount);

    /** Return <TRUE/> when the current transferable in the current state of
        the slidesorter is acceptable to be pasted.  For this the
        transferable has to
        a) exist,
        b) contain one or more regular draw pages, no master pages.
        When master pages are involved, either in the transferable or in the
        slide sorter (by it displaying master pages) the drop of the
        transferable is not accepted.  The reason is the missing
        implementation of proper handling master pages copy-and-paste.
    */
    enum DropType { DT_PAGE, DT_SHAPE, DT_NONE };
    DropType IsDropAccepted (void) const;

    /** This method contains the code for AcceptDrop() and ExecuteDrop() shapes.
        There are only minor differences for the two cases at this level.
        @param eCommand
            This parameter specifies whether to do a AcceptDrop() or
            ExecuteDrop().
        @param rPosition
            Since the event is given as void pointer we can not take the
            mouse position from it.  The caller has to supply it in this
            parameter.
        @param pDropEvent
            Event though the AcceptDropEvent and ExecuteDropEvent are very
            similar they do not have a common base class.  Because of that
            we have to use a void* to pase these structs.
        @param nPage
            When the page number is given as 0xffff then it is replaced by
            the number of the page at the mouse position.  If the mouse is
            not over a page then neither AcceptDrop() nor ExecuteDrop() are
            executed.
    */
    enum DropCommand { DC_ACCEPT, DC_EXECUTE };
    sal_Int8 ExecuteOrAcceptShapeDrop (
        DropCommand eCommand,
        const Point& rPosition,
        const void* pDropEvent ,
        DropTargetHelper& rTargetHelper,
        ::sd::Window* pTargetWindow,
        USHORT nPage,
        USHORT nLayer);
};

} } } // end of namespace ::sd::slidesorter::controller

#endif

