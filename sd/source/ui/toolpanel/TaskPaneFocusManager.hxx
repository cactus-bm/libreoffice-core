/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: TaskPaneFocusManager.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:33:20 $
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

#ifndef SD_TOOLPANEL_FOCUS_MANAGER_HXX
#define SD_TOOLPANEL_FOCUS_MANAGER_HXX

#ifndef _LINK_HXX
#include <tools/link.hxx>
#endif

#include <memory>

class KeyCode;
class VclSimpleEvent;
class Window;

namespace sd { namespace toolpanel {

/** On certain key presses the focus is moved from one window to another.
    For this to work every window that wants its focus managed has to
    register or be registered and tell where to put the focus on what key
    press.
*/
class FocusManager
{
public:
    /** Return an instance of the focus manager.
    */
    static FocusManager& Instance (void);

    /** Register a link from one window to another so that any time the
        specified key is pressed while the source window is focused, the
        focus is transferred to the target window.
        @param pSource
            The window from which the focus will be transferred.
        @param pTarget
            The window to which the focus will be transferred.
        @param rKey
            The key for which the focus is transferred from the source
            window to the target window.
    */
    void RegisterLink (
        ::Window* pSource,
        ::Window* pTarget,
        const KeyCode& rKey);

    /** Register a link that will move the focus from the source window to
        the target window when the source window is focused and KEY_ESCAPE
        is pressed.
        @param pSource
            The window from which the focus will be transferred.
        @param pTarget
            The window to which the focus will be transferred.
    */
    void RegisterUpLink (::Window* pSource, ::Window* pTarget);

    /** Register a link that will move the focus from the source window to
        the target window when the source window is focused and KEY_RETURN
        is pressed.
        @param pSource
            The window from which the focus will be transferred.
        @param pTarget
            The window to which the focus will be transferred.
    */
    void RegisterDownLink (::Window* pSource, ::Window* pTarget);

    /** Remove all links from the source window to the target window.  When
        there are links from the target window to the source window then
        these are not touced.
    */
    void RemoveLinks (
        ::Window* pSource,
        ::Window* pTarget);

    /** Let the focus manager transfer the focus from the specified source
        window to a target window that is determined according the the
        registered links and the given key code.
        When there is no rule for this combination of source window and key
        code then the focus stays where it is.
    */
    bool TransferFocus (::Window* pSource, const KeyCode& rCode);

private:
    static FocusManager* spInstance;
    class LinkMap;
    ::std::auto_ptr<LinkMap> mpLinks;

    FocusManager (void);
    ~FocusManager (void);

    /** Remove all links from or to the given window.
    */
    void RemoveLinks (::Window* pWindow);

    /** Unregister as event listener from the given window when there are no
        links from this window anymore.
    */
    void RemoveUnusedEventListener (::Window* pWindow);

    /** Listen for key events and on KEY_RETURN go down and on
        KEY_ESCAPE go up.
    */
    DECL_LINK(WindowEventListener, VclSimpleEvent*);
};

} } // end of namespace ::sd::toolpanel

#endif
