/*************************************************************************
 *
 *  $RCSfile: _XControlAccess.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change:$Date: 2003-01-27 18:14:09 $
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

package ifc.view;

import com.sun.star.awt.XControl;
import com.sun.star.awt.XControlModel;
import com.sun.star.drawing.XControlShape;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XDrawPageSupplier;
import com.sun.star.drawing.XShape;
import com.sun.star.drawing.XShapes;
import com.sun.star.lang.XComponent;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.view.XControlAccess;
import lib.MultiMethodTest;
import lib.Status;
import lib.StatusException;
import util.FormTools;

/**
 * Testing <code>com.sun.star.view.XControlAccess</code>
 * interface methods :
 * <ul>
 *  <li><code> getControl()</code></li>
 * </ul> <p>
 * This test needs the following object relations :
 * <ul>
 *  <li> <code>'DOCUMENT'</code> (of type <code>XComponent</code>):
 *   the document, which controller is tested here. Is used
 *   for adding a component (such as CommandButton) and obtaining
 *   its control via tested interface. </li>
 * <ul> <p>
 * Test is <b> NOT </b> multithread compilant. <p>
 * @see com.sun.star.view.XControlAccess
 */
public class _XControlAccess extends MultiMethodTest {

    public XControlAccess oObj = null;

    /**
     * Retrieves a document from relation, then using it adds
     * a CommandButton to a document and obtains button model.
     * After that button control is tried to get using the
     * tested interface. <p>
     * Has <b> OK </b> status if non <code>null</code> control
     * is returned. <p>
     * @throws StatusException If the relation was not found.
     */
    public void _getControl(){
        boolean bResult = true;
        try {
            XComponent oDoc = (XComponent)tEnv.getObjRelation("DOCUMENT");
            if (oDoc == null) {
                throw new StatusException
                    (Status.failed("NO 'DOCUMENT' ObjRelation!"));
            }

            XDrawPageSupplier oDPS = (XDrawPageSupplier)
                UnoRuntime.queryInterface(XDrawPageSupplier.class, oDoc);
            XDrawPage oDP = oDPS.getDrawPage();
            XShapes shapes = (XShapes) UnoRuntime.queryInterface
                (XShapes.class, oDP);
            XShape button = FormTools.createControlShape
                (oDoc, 100, 100, 10000, 50000, "CommandButton");
            shapes.add(button);

            XControlModel CM = ((XControlShape)button).getControl();
            log.println("Getting ControlModel "
                + ((CM == null) ? "FAILED" : "OK"));

            XControl oControl = oObj.getControl(CM);
            log.println("Getting Control " + ((CM == null) ? "FAILED" : "OK"));

            bResult &= oControl != null;
        } catch (com.sun.star.container.NoSuchElementException e) {
            log.println("Exception occured calling the method: " + e);
            bResult = false;
        }
        tRes.tested("getControl()", bResult);
    }
}

