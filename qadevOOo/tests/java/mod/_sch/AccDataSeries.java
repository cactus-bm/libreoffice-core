/*************************************************************************
 *
 *  $RCSfile: AccDataSeries.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Date: 2003-01-27 18:17:20 $
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
package mod._sch;

import lib.TestCase;
import lib.TestParameters;
import java.io.PrintWriter;
import lib.TestEnvironment;
import com.sun.star.uno.XInterface;
import util.SOfficeFactory;
import lib.StatusException;
import com.sun.star.frame.XController;
import util.AccessibilityTools;
import drafts.com.sun.star.accessibility.AccessibleRole;
import drafts.com.sun.star.accessibility.XAccessibleContext;
import drafts.com.sun.star.accessibility.XAccessibleValue;
import drafts.com.sun.star.accessibility.XAccessibleComponent;
import com.sun.star.beans.XPropertySet;
import com.sun.star.frame.XModel;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.awt.XWindow;
import drafts.com.sun.star.accessibility.XAccessible;
import util.utils;
import util.SOfficeFactory;
import com.sun.star.chart.XChartDocument;
import com.sun.star.drawing.XShape;
import com.sun.star.awt.Rectangle;
import com.sun.star.awt.PosSize;
import com.sun.star.view.XSelectionSupplier;

public class AccDataSeries extends TestCase {

    XChartDocument xChartDoc = null;

    protected TestEnvironment createTestEnvironment(
        TestParameters Param, PrintWriter log) {

        if (xChartDoc != null) xChartDoc.dispose();
        log.println( "creating a chart document" );
        SOfficeFactory SOF = SOfficeFactory.getFactory( Param.getMSF());
        try {
            log.println( "creating a chartdocument" );
            xChartDoc = SOF.createChartDoc(null);
        } catch (com.sun.star.uno.Exception e) {
            // Some exception occures.FAILED
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }

        XInterface oObj = null;

        XModel aModel = (XModel)
            UnoRuntime.queryInterface(XModel.class, xChartDoc);

        AccessibilityTools at = new AccessibilityTools();

        XWindow xWindow = at.getCurrentWindow(Param.getMSF(), aModel);
        XAccessible xRoot = at.getAccessibleObject(xWindow);

        //at.printAccessibleTree(log,xRoot);

        XAccessibleContext cont = at.getAccessibleObjectForRole(
                xRoot, AccessibleRole.SHAPE, "", "AccDataSeries");

        oObj = cont;

        log.println("ImplementationName " + utils.getImplName(oObj));
        log.println("AccessibleName " + cont.getAccessibleName());

        TestEnvironment tEnv = new TestEnvironment(oObj);

        final XAccessibleComponent acc = (XAccessibleComponent)
                UnoRuntime.queryInterface(
                    XAccessibleComponent.class,oObj);
        tEnv.addObjRelation("EventProducer",
            new ifc.accessibility._XAccessibleEventBroadcaster.EventProducer() {
                public void fireEvent() {
                        acc.grabFocus();
                }
            });

        return tEnv;

    }

    /**
    * Called while disposing a <code>TestEnvironment</code>.
    * Disposes text document.
    * @param tParam test parameters
    * @param tEnv the environment to cleanup
    * @param log writer to log information while testing
    */
    protected void cleanup( TestParameters Param, PrintWriter log) {
        log.println("dispose chart document");
        xChartDoc.dispose();
    }

}