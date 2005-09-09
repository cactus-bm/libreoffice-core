/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: LayoutManager.java,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:20:59 $
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

package mod._fwk;

import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.frame.XController;
import com.sun.star.frame.XFrame;
import com.sun.star.frame.XModel;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XSingleServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.ui.XModuleUIConfigurationManagerSupplier;
import ifc.ui._XUIConfiguration;
import java.io.PrintWriter;
import com.sun.star.lang.EventObject;
import com.sun.star.text.XText;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.util.XCloseable;
import com.sun.star.ui.ConfigurationEvent;
import com.sun.star.ui.XUIConfigurationManager;
import com.sun.star.ui.XUIConfigurationManagerSupplier;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.WriterTools;

/**
 */
public class LayoutManager extends TestCase {
    XInterface xManager = null;
    XTextDocument xTextDoc;

    /**
     * Cleanup: close the created document
     * @param tParam The test parameters.
     * @param The log writer.
     * @return The test environment.
     */
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
     * Create test environment:
     * <ul>
     * <li>Create test doc</li>
     * <li>Get the frame</li>
     * <li>Get the LayoutManager from the frame</li>
     * </ul>
     * @param tParam The test parameters.
     * @param The log writer.
     * @return The test environment.
     */
    protected TestEnvironment createTestEnvironment(TestParameters tParam, PrintWriter log) {
        TestEnvironment tEnv = null;
        XMultiServiceFactory xMSF = (XMultiServiceFactory)tParam.getMSF();

        log.println("Creating instance...");

        xTextDoc = WriterTools.createTextDoc(xMSF);

        XText xText = xTextDoc.getText();
        XTextCursor xTextCursor = xText.createTextCursor();

        for (int i = 0; i < 11; i++) {
            xText.insertString(xTextCursor, "A sample text and why not? ", false);
        }

        XFrame xFrame = xTextDoc.getCurrentController().getFrame();
        XPropertySet xProp = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xFrame);
        try {
            Object any = xProp.getPropertyValue("LayoutManager");
            xManager = (XInterface)UnoRuntime.queryInterface(XInterface.class, any);
        }
        catch(com.sun.star.beans.UnknownPropertyException e) {
            e.printStackTrace(log);
            throw new StatusException("Could not get property 'LayoutManager' from the current frame.", e);
        }
        catch(com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log);
            throw new StatusException("Could not get property 'LayoutManager' from the current frame.", e);
        }

        // just to make sure, it's the right one.
        log.println("TestObject: " + util.utils.getImplName(xManager));
        tEnv = new TestEnvironment(xManager);

        tEnv.addObjRelation("XLayoutManager.TextDoc", xTextDoc);
        tEnv.addObjRelation("XLayoutManager.Frame",xFrame);

        return tEnv;
    }
}


