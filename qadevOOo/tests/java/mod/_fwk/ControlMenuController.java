/*************************************************************************
 *
 *  $RCSfile: ControlMenuController.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Date: 2004-02-25 18:13:01 $
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

package mod._fwk;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.util.XCloseable;
import java.io.PrintWriter;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.WriterTools;
import util.utils;

/**
 */
public class ControlMenuController extends TestCase {
    XInterface oObj = null;
    XTextDocument xTextDoc;

    /**
     * Cleanup: close the created document
     * @param tParam The test parameters.
     * @param The log writer.
     * @return The test environment.
     *
    protected void cleanup(TestParameters tParam, PrintWriter log) {
        log.println("    disposing xTextDoc ");

        try {
            XCloseable closer = (XCloseable) UnoRuntime.queryInterface(
            XCloseable.class, xTextDoc);
            closer.close(true);
        } catch (com.sun.star.util.CloseVetoException e) {
            log.println("couldn't close document");
        } catch (com.sun.star.lang.DisposedException e) {
            log.println("couldn't close document");
        }
    }

    /**
     * Create test environment.
     */
    protected TestEnvironment createTestEnvironment(TestParameters tParam, PrintWriter log) {
        TestEnvironment tEnv = null;
        XMultiServiceFactory xMSF = (XMultiServiceFactory)tParam.getMSF();
        try {
            oObj = (XInterface)xMSF.createInstance("com.sun.star.comp.framework.ControlMenuController");
        }
        catch(com.sun.star.uno.Exception e) {
            e.printStackTrace(log);
            throw new StatusException("Could not create object 'ControlMenuController'", e);
        }


/*        log.println("Creating instance...");

        xTextDoc = WriterTools.createTextDoc(xMSF);

/*        XText xText = xTextDoc.getText();
        XTextCursor xTextCursor = xText.createTextCursor();

        for (int i = 0; i < 11; i++) {
            xText.insertString(xTextCursor, "A sample text and why not? ", false);
        }

        XModel xModel = (XModel)UnoRuntime.queryInterface(XModel.class, xTextDoc);

        // the supplier is available from the model
        XUIConfigurationManagerSupplier xSupplier =
                (XUIConfigurationManagerSupplier)UnoRuntime.queryInterface(
                XUIConfigurationManagerSupplier.class, xModel);

        xManager = xSupplier.getUIConfigurationManager();

        // just to make sure, it's the right one.
        log.println("TestObject: " + util.utils.getImplName(xManager));
        tEnv = new TestEnvironment(xManager); */
        log.println("TestObject: " + utils.getImplName(oObj));
        tEnv = new TestEnvironment(oObj);
        return tEnv;
    }

}
