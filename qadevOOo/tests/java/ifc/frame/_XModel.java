/*************************************************************************
 *
 *  $RCSfile: _XModel.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change:$Date: 2003-01-27 18:10:17 $
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

package ifc.frame;

import com.sun.star.beans.PropertyValue;
import com.sun.star.frame.XController;
import com.sun.star.frame.XModel;
import com.sun.star.view.XSelectionSupplier;
import lib.MultiMethodTest;

/**
* Testing <code>com.sun.star.frame.XModel</code>
* interface methods:
* <ul>
*  <li><code> attachResource() </code></li>
*  <li><code> connectController() </code></li>
*  <li><code> disconnectController() </code></li>
*  <li><code> getArgs() </code></li>
*  <li><code> getCurrentController() </code></li>
*  <li><code> getCurrentSelection() </code></li>
*  <li><code> getURL() </code></li>
*  <li><code> hasControllersLocked() </code></li>
*  <li><code> lockControllers() </code></li>
*  <li><code> setCurrentController() </code></li>
*  <li><code> unlockControllers() </code></li>
* </ul><p>
* This test needs the following object relations :
* <ul>
*  <li> <code>'CONT2'</code> (of type <code>XController</code>):
*  a controller used as an argument for several test methods </li>
*  <li> <code>'TOSELECT'</code> (of type <code>Object</code>):
*   something, we want to select from document </li>
*  <li> <code>'SELSUPP'</code> (of type <code>XSelectionSupplier</code>):
*   supplier, we use to select something in a document </li>
* </ul> <p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.frame.XModel
*/
public class _XModel extends MultiMethodTest {
    public XModel oObj = null;
    boolean    result = true;

    /**
    * Test calls the method. <p>
    * Has <b> OK </b> status if the method returns true.
    */
    public void _attachResource() {
        log.println( "opening DataSourceBrowser" );
        PropertyValue[] noArgs = new PropertyValue[0];
        result = oObj.attachResource(".component:DB/DataSourceBrowser",noArgs);
        tRes.tested("attachResource()", result);
    }

    /**
    * After obtaining object relation 'CONT2' and storing old controller,
    * test calls the method, then result is checked.<p>
    * Has <b> OK </b> status if controller, gotten after method call is not
    * equal to a previous controller.
    */
    public void _connectController() {
        XController cont2 = (XController) tEnv.getObjRelation("CONT2");
        XController old = oObj.getCurrentController();
        result = false;
        if (cont2 == null) {
            log.println("No controller no show");
        } else {
            oObj.connectController(cont2);
            result = (oObj.getCurrentController() != old);
        }
        tRes.tested("connectController()", result);
    }

    /**
    * After obtaining object relation 'CONT2', test calls the method,
    * then result is checked.<p>
    * Has <b> OK </b> status if controller, gotten after method call is not
    * equal to a controller we use as an argument to method.
    */
    public void _disconnectController() {
        XController cont2 = (XController) tEnv.getObjRelation("CONT2");
        result = false;
        if (cont2 == null) {
            log.println("No controller no show");
        } else {
            oObj.disconnectController(cont2);
            result = (oObj.getCurrentController() != cont2);
        }
        tRes.tested("disconnectController()", result);
    }

    /**
    * Test calls the method. <p>
    * Has <b> OK </b> status if the method does not return null.
    */
    public void _getArgs() {
        tRes.tested("getArgs()", oObj.getArgs() != null);
    }

    /**
    * Test calls the method. <p>
    * Has <b> OK </b> status if the method does not return null.
    */
    public void _getCurrentController() {
        XController ctrl = oObj.getCurrentController();
        tRes.tested("getCurrentController()", ctrl != null);
    }

    /**
    * After obtaining object relations 'SELSUPP' and 'TOSELECT', test prepares
    * selection and calls the method. <p>
    * Has <b> OK </b> status if the method does not return null.
    */
    public void _getCurrentSelection() {
        XSelectionSupplier selsupp = (XSelectionSupplier)
            tEnv.getObjRelation("SELSUPP");
        Object toSelect = tEnv.getObjRelation("TOSELECT");
        result = false;
        if ( selsupp == null ) {
            log.println("No Selection Supplier no show");
        } else {
            try {
                selsupp.select(toSelect);
            } catch (com.sun.star.lang.IllegalArgumentException e) {
                log.println("Exception occured while select:");
                e.printStackTrace(log);
                return;
            }
            Object sel = oObj.getCurrentSelection();
            result = (sel != null);
        }

        tRes.tested("getCurrentSelection()", result);
    }

    /**
    * Test calls the method. <p>
    * Has <b> OK </b> status if the method does not return null.
    */
    public void _getURL() {
        String url = oObj.getURL();
        log.println("The url: " + url);
        tRes.tested("getURL()", url != null);
    }

    /**
    * if controller is not locked, test calls the method. <p>
    * Has <b> OK </b> status if the method returns true. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> lockControllers() </code> : locks controllers </li>
    * </ul>
    */
    public void _hasControllersLocked() {
        requiredMethod("lockControllers()");
        tRes.tested("hasControllersLocked()", oObj.hasControllersLocked());
    }

    /**
    * Test calls the method, then result is checked.<p>
    * Has <b> OK </b> status if method locks controllers.
    */
    public void _lockControllers() {
        oObj.lockControllers();
        result = oObj.hasControllersLocked();
        tRes.tested("lockControllers()", result);
    }

    /**
    * After obtaining object relation 'CONT2' and storing old controller,
    * controller cont2 is connected, then this controller is
    * setting as current.
    * Sets the old controller as current.
    * <p>Has <b> OK </b> status if set and gotten controllers are not equal.
    */
    public void _setCurrentController() {
        XController cont2 = (XController) tEnv.getObjRelation("CONT2");
        XController old = oObj.getCurrentController();
        result = false;
        if (cont2 == null) {
            log.println("No controller no show");
        } else {
            oObj.connectController(cont2);
            try {
                oObj.setCurrentController(cont2);
            } catch (com.sun.star.container.NoSuchElementException e) {
                log.print("Cannot set current controller:");
                e.printStackTrace(log);
            }
            result = (oObj.getCurrentController() != old);
            try {
                oObj.setCurrentController(old);
            } catch (com.sun.star.container.NoSuchElementException e) {
                log.print("Cannot set current controller:");
                e.printStackTrace(log);
            }
        }
        tRes.tested("setCurrentController()", result);
    }

    /**
    * Test calls the method. <p>
    * Has <b> OK </b> status if method unlocks controllers.
    * <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> hasControllersLocked() </code> : checks if controllers are
    * locked </li>
    * </ul>
    */
    public void _unlockControllers() {
        requiredMethod("hasControllersLocked()");
        oObj.unlockControllers();
        result = ! oObj.hasControllersLocked();
        tRes.tested("unlockControllers()", result);
    }

}


