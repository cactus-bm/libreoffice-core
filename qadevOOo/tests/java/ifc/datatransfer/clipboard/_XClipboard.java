/*************************************************************************
 *
 *  $RCSfile: _XClipboard.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change:$Date: 2003-01-27 18:08:49 $
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

package ifc.datatransfer.clipboard;

import com.sun.star.datatransfer.DataFlavor;
import com.sun.star.datatransfer.XTransferable;
import com.sun.star.datatransfer.clipboard.XClipboard;
import com.sun.star.datatransfer.clipboard.XClipboardOwner;
import lib.MultiMethodTest;
import lib.StatusException;

/**
* Testing <code>com.sun.star.datatransfer.clipboard.XClipboard</code>
* interface methods :
* <ul>
*  <li><code> getContents()</code></li>
*  <li><code> setContents()</code></li>
*  <li><code> getName()</code></li>
* </ul> <p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.datatransfer.clipboard.XClipboard
*/
public class _XClipboard extends MultiMethodTest {

    public XClipboard oObj;

    MyTransferable myTransferable1;
    MyTransferable myTransferable2;

    MyOwner myOwner;

    /**
     * <code>XClipboardOwner</code> interface implementation which
     * stores parameters passed to <code>lostOwnership</code> method.
     */
    class MyOwner implements XClipboardOwner {
        XClipboard board;
        XTransferable contents;

        public void lostOwnership(XClipboard board, XTransferable contents) {
            this.board = board;
            this.contents = contents;
        }

    }

    /**
     * Simpliest <code>XTransferable</code> interface implementation.
     */
    class MyTransferable implements XTransferable {
        public Object getTransferData(DataFlavor dataFlavor) {
            return "";
        }

        public com.sun.star.datatransfer.DataFlavor[] getTransferDataFlavors() {
            return new DataFlavor[0];
        }

        public boolean isDataFlavorSupported(DataFlavor dataFlavor) {
            return false;
        }

    }

    /**
     * Initially sets the content of the clipboard.
     */
    public void before() {
        oObj.setContents(myTransferable1 = new MyTransferable(),
                myOwner = new MyOwner());
    }

    /**
     * Gets the name of the clipboard. <p>
     * Has <b>OK</b> status if not <code>null</code> value returned.
     */
    public void _getName() {
        String name = oObj.getName();
        tRes.tested("getName()", oObj.getName() != null);
    }

    /**
     * Gets the contents of the clipboard. <p>
     * Has <b>OK</b> status if the content obtained is equal to content
     * set in <code>before</code> method.
     */
    public void _getContents() {
        tRes.tested("getContents()", oObj.getContents() == myTransferable1);
    }

    /**
    * Sets new contents for the clipboard. Then checks if it was set,
    * and if <code>lostOwnerShip()</code> notification of the prevoius
    * contents was called with appropriate parameters.<p>
    * Has <b> OK </b> status if <code>getContents</code> returns the same
    * object which is set, and notification was received.
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> getContents() </code> : for right testing order </li>
    * </ul>
    */
    public void _setContents() {
        requiredMethod("getContents()");
        myTransferable2 = new MyTransferable();

        oObj.setContents(myTransferable2, new MyOwner());

        log.println("sleeping for 1 second");

        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            log.println("interrupted");
            e.printStackTrace(log);
            throw new StatusException("Operation interrupted", e);
        }

        tRes.tested("setContents()", oObj.getContents() == myTransferable2);
    }
}
