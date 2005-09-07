/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: FocusTraversalPolicy.java,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 15:37:38 $
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

package org.openoffice.java.accessibility;


public class FocusTraversalPolicy extends java.awt.FocusTraversalPolicy {

    protected javax.accessibility.Accessible getSelectedAccessibleChild(javax.accessibility.Accessible a) {
        javax.accessibility.AccessibleContext ac = a.getAccessibleContext();
        if (ac != null) {
            javax.accessibility.AccessibleSelection as = ac.getAccessibleSelection();
            if (as != null) {
                return as.getAccessibleSelection(0);
            }
        }
        return null;
    }

    /** Returns the Component that should receive the focus after aComponent */
    public java.awt.Component getComponentAfter(java.awt.Container focusCycleRoot,
        java.awt.Component aComponent) {
        return null;
    }

    /** Returns the Component that should receive the focus before aComponent */
    public java.awt.Component getComponentBefore(java.awt.Container focusCycleRoot,
        java.awt.Component aComponent) {
        return null;
    }

    /** Returns the default Component to focus */
    public java.awt.Component getDefaultComponent(java.awt.Container focusCycleRoot) {
        // getDefaultComponent must not return null for Windows to make them focusable.
        if (focusCycleRoot instanceof NativeFrame) {
            java.awt.Component c = ((NativeFrame) focusCycleRoot).getInitialComponent();
            if (c != null) {
                return c;
            }
        }

        if (focusCycleRoot instanceof javax.accessibility.Accessible) {
            return (java.awt.Component) getSelectedAccessibleChild((javax.accessibility.Accessible) focusCycleRoot);
        }
        return null;
    }

    /** Returns the first Component in the traversal cycle */
    public java.awt.Component getFirstComponent(java.awt.Container focusCycleRoot) {
        return null;
    }

    /** Returns the Component that should receive the focus when a Window is made visible for the first time */
    public java.awt.Component getInitialComponent(java.awt.Window window) {
        if (window instanceof NativeFrame) {
            return ((NativeFrame) window).getInitialComponent();
        }
        return null;
    }

    /** Returns the last Component in the traversal cycle */
    public java.awt.Component getLastComponent(java.awt.Container focusCycleRoot) {
        return null;
    }
}
