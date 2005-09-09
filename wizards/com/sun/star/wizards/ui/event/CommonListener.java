/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: CommonListener.java,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 09:50:19 $
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
 ************************************************************************/package com.sun.star.wizards.ui.event;

import com.sun.star.awt.*;
import com.sun.star.lang.EventObject;

/**
 *
 * @author  rpiterman
 */
public class CommonListener extends AbstractListener implements XActionListener, XItemListener, XTextListener, EventNames, XWindowListener, XMouseListener, XFocusListener, XKeyListener {

    /** Creates a new instance of CommonListener */
    public CommonListener() {
    }

    /**
     * Implementation of com.sun.star.awt.XActionListener
     */
    public void actionPerformed(com.sun.star.awt.ActionEvent actionEvent) {
        invoke(getEventSourceName(actionEvent), EVENT_ACTION_PERFORMED, actionEvent);
    }

    public void disposing(com.sun.star.lang.EventObject eventObject) {
    }

    /**
     * Implementation of com.sun.star.awt.XItemListener
     */
    public void itemStateChanged(ItemEvent itemEvent) {
        invoke(getEventSourceName(itemEvent), EVENT_ITEM_CHANGED, itemEvent);
    }

    /**
     * Implementation of com.sun.star.awt.XTextListener
     */
    public void textChanged(TextEvent textEvent) {
        invoke(getEventSourceName(textEvent), EVENT_TEXT_CHANGED, textEvent);
    }

    /**
     * @see com.sun.star.awt.XWindowListener#windowResized(com.sun.star.awt.WindowEvent)
     */
    public void windowResized(WindowEvent event) {
        invoke(getEventSourceName(event), EVENT_WINDOW_RESIZED, event);
    }

    /**
     * @see com.sun.star.awt.XWindowListener#windowMoved(com.sun.star.awt.WindowEvent)
     */
    public void windowMoved(WindowEvent event) {
        invoke(getEventSourceName(event), EVENT_WINDOW_MOVED, event);
    }

    /**
     * @see com.sun.star.awt.XWindowListener#windowShown(com.sun.star.lang.EventObject)
     */
    public void windowShown(EventObject event) {
        invoke(getEventSourceName(event), EVENT_WINDOW_SHOWN, event);
    }

    /**
     * @see com.sun.star.awt.XWindowListener#windowHidden(com.sun.star.lang.EventObject)
     */
    public void windowHidden(EventObject event) {
        invoke(getEventSourceName(event), EVENT_WINDOW_HIDDEN, event);
    }

    /**
     * @see com.sun.star.awt.XMouseListener#mousePressed(com.sun.star.awt.MouseEvent)
     */
    public void mousePressed(MouseEvent event) {
        invoke(getEventSourceName(event), EVENT_MOUSE_PRESSED, event);
    }

    /**
     * @see com.sun.star.awt.XMouseListener#mouseReleased(com.sun.star.awt.MouseEvent)
     */
    public void mouseReleased(MouseEvent event) {
        invoke(getEventSourceName(event), EVENT_KEY_RELEASED, event);
    }

    /**
     * @see com.sun.star.awt.XMouseListener#mouseEntered(com.sun.star.awt.MouseEvent)
     */
    public void mouseEntered(MouseEvent event) {
        invoke(getEventSourceName(event), EVENT_MOUSE_ENTERED, event);
    }

    /**
     * @see com.sun.star.awt.XMouseListener#mouseExited(com.sun.star.awt.MouseEvent)
     */
    public void mouseExited(MouseEvent event) {
        invoke(getEventSourceName(event), EVENT_MOUSE_EXITED, event);
    }

    /**
     * @see com.sun.star.awt.XFocusListener#focusGained(com.sun.star.awt.FocusEvent)
     */
    public void focusGained(FocusEvent event) {
        invoke(getEventSourceName(event), EVENT_FOCUS_GAINED, event);
    }

    /**
     * @see com.sun.star.awt.XFocusListener#focusLost(com.sun.star.awt.FocusEvent)
     */
    public void focusLost(FocusEvent event) {
        invoke(getEventSourceName(event), EVENT_FOCUS_LOST, event);
    }

    /**
     * @see com.sun.star.awt.XKeyListener#keyPressed(com.sun.star.awt.KeyEvent)
     */
    public void keyPressed(KeyEvent event) {
        invoke(getEventSourceName(event), EVENT_KEY_PRESSED, event);
    }

    /**
     * @see com.sun.star.awt.XKeyListener#keyReleased(com.sun.star.awt.KeyEvent)
     */
    public void keyReleased(KeyEvent event) {
        invoke(getEventSourceName(event), EVENT_KEY_RELEASED, event);
    }

}
