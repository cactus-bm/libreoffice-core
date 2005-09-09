/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: AccessiblePresentationGraphicShape.java,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 03:33:14 $
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

package mod._svx;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.AccessibilityTools;
import util.DrawTools;
import util.SOfficeFactory;
import util.utils;

import com.sun.star.accessibility.AccessibleRole;
import com.sun.star.accessibility.XAccessible;
import com.sun.star.awt.Size;
import com.sun.star.awt.XWindow;
import com.sun.star.beans.XPropertySet;
import com.sun.star.drawing.XShape;
import com.sun.star.frame.XModel;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

public class AccessiblePresentationGraphicShape extends TestCase {

    XComponent xDoc;
    XModel aModel;

    protected void initialize( TestParameters tParam, PrintWriter log ) {

        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF() );

        try {
            log.println( "creating a drawdoc" );
            xDoc = SOF.createImpressDoc(null);
            aModel = (XModel)
                UnoRuntime.queryInterface(XModel.class, xDoc);

        } catch ( com.sun.star.uno.Exception e ) {
            // Some exception occures.FAILED
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }
    }

    /**
     * Disposes the Draw document loaded before.
     */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xDrawDoc " );
        util.DesktopTools.closeDoc(xDoc);
    }

    protected TestEnvironment createTestEnvironment
            (TestParameters tParam, PrintWriter log) {

        XInterface oObj = null;
        XShape oShape = null;

        // creation of testobject here
        // first we write what we are intend to do to log file
        log.println( "creating a test environment" );

        XMultiServiceFactory docMSF = (XMultiServiceFactory)
            UnoRuntime.queryInterface(XMultiServiceFactory.class, xDoc);
        try {
            oShape = (XShape) UnoRuntime.queryInterface(XShape.class,
                docMSF.createInstance
                ("com.sun.star.presentation.GraphicObjectShape"));
        } catch (com.sun.star.uno.Exception e) {
            throw new StatusException("couldn't create component", e);
        }

        DrawTools.getShapes(DrawTools.getDrawPage(xDoc,0)).add(oShape);

        XPropertySet oShapeProps = (XPropertySet)
                            UnoRuntime.queryInterface(XPropertySet.class,oShape);
        try {
            oShapeProps.setPropertyValue(
                "GraphicURL",util.utils.getFullTestURL("space-metal.jpg"));
            oShapeProps.setPropertyValue("IsEmptyPresentationObject", new Boolean(false));
        } catch (com.sun.star.lang.WrappedTargetException e) {
        } catch (com.sun.star.lang.IllegalArgumentException e) {
        } catch (com.sun.star.beans.PropertyVetoException e) {
        } catch (com.sun.star.beans.UnknownPropertyException e) {
        }

        //DrawTools.getShapes(DrawTools.getDrawPage(xDoc,0)).add(oShape);

        AccessibilityTools at = new AccessibilityTools();

        XWindow xWindow = at.getCurrentWindow ((XMultiServiceFactory)tParam.getMSF(),aModel);
        XAccessible xRoot = at.getAccessibleObject(xWindow);

        oObj = at.getAccessibleObjectForRole
            (xRoot, AccessibleRole.SHAPE, "GraphicObject");

        // create test environment here
        TestEnvironment tEnv = new TestEnvironment( oObj );

        final XShape fShape = oShape ;
        tEnv.addObjRelation("EventProducer",
            new ifc.accessibility._XAccessibleEventBroadcaster.EventProducer() {
                public void fireEvent() {
                    try {
                        Size size = fShape.getSize();
                        size.Width += 100;
                        fShape.setSize(size);
                    } catch(com.sun.star.beans.PropertyVetoException e) {}
                }
            });

        log.println("Implementation Name: " + utils.getImplName(oObj));

        return tEnv;
    } // finish method getTestEnvironment

}

