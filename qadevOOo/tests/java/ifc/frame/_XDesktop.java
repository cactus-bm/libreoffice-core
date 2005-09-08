/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _XDesktop.java,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:00:26 $
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

package ifc.frame;

import lib.MultiMethodTest;

import com.sun.star.container.NoSuchElementException;
import com.sun.star.container.XEnumeration;
import com.sun.star.container.XEnumerationAccess;
import com.sun.star.frame.XDesktop;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;
import com.sun.star.uno.XInterface;

/**
* Testing <code>com.sun.star.frame.XDesktop</code>
* interface methods:
* <ul>
*  <li><code> getComponents() </code></li>
*  <li><code> terminate() </code></li>
*  <li><code> addTerminateListener() </code></li>
*  <li><code> removeTerminateListener() </code></li>
*  <li><code> getCurrentComponent() </code></li>
*  <li><code> getCurrentFrame() </code></li>
* </ul><p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.frame.XDesktop
*/
public class _XDesktop extends MultiMethodTest {
    public XDesktop oObj = null; // oObj filled by MultiMethodTest

    /**
    * Test calls the method. Then elements enumeration is created and tested.<p>
    * Has <b> OK </b> status if no exceptions were thrown.
    */
    public void _getComponents() {
        XEnumerationAccess xComps = oObj.getComponents();
        XEnumeration xEnum = xComps.createEnumeration();
        boolean result = false;

        try {
            for (; xEnum.hasMoreElements();) {
                XInterface xInt = null;
                try {
                    xInt = (XInterface) AnyConverter.toObject(
                            new Type(XInterface.class), xEnum.nextElement());
                } catch (com.sun.star.lang.IllegalArgumentException iae) {
                    log.println("Can't convert any");
                }
            }
            result = true;
        } catch (WrappedTargetException e) {
            log.println("Couldn't get a component : " + e.getMessage());
            e.printStackTrace();
        } catch (NoSuchElementException e) {
            log.println("Couldn't get a component : " + e.getMessage());
            e.printStackTrace();
        }
        tRes.tested("getComponents()", result);
    }

    /**
    * Cannot test the method because it requires
    * terminating StarOffice. Will add real test later.
    */
    public void _terminate() {
        tRes.tested("terminate()", true);
    }

    /**
    * Cannot test the method because of terminate().
    * Will add real test later.
    */
    public void _addTerminateListener() {
        tRes.tested("addTerminateListener()", true);
    }

    /**
    * Cannot test the method because of terminate().
    * Will add real test later.
    */
    public void _removeTerminateListener() {
        tRes.tested("removeTerminateListener()", true);
    }

    /**
    * Test calls the method. <p>
    * Has <b> OK </b> status if the method does not return null.
    */
    public void _getCurrentComponent() {
        tRes.tested("getCurrentComponent()",
            oObj.getCurrentComponent() != null);
    }

    /**
    * Test calls the method. <p>
    * Has <b> OK </b> status if the method does not return null.
    */
    public void _getCurrentFrame() {
        tRes.tested("getCurrentFrame()", oObj.getCurrentFrame() != null);
    }

}

