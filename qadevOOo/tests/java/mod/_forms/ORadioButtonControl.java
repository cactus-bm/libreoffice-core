/*************************************************************************
 *
 *  $RCSfile: ORadioButtonControl.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change:$Date: 2003-05-27 12:45:49 $
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

package mod._forms;

import com.sun.star.awt.XControl;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.awt.XControlModel;
import com.sun.star.awt.XDevice;
import com.sun.star.awt.XGraphics;
import com.sun.star.awt.XToolkit;
import com.sun.star.awt.XWindow;
import com.sun.star.awt.XWindowPeer;
import com.sun.star.drawing.XControlShape;
import com.sun.star.drawing.XShape;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.view.XControlAccess;
import java.io.PrintWriter;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.FormTools;
import util.SOfficeFactory;
import util.WriterTools;


/**
 * Test for object which is represented by default controller
 * of the <code>com.sun.star.form.component.RadioButton</code>
 * component. <p>
 *
 * Object implements the following interfaces :
 * <ul>
 *  <li> <code>com::sun::star::lang::XComponent</code></li>
 *  <li> <code>com::sun::star::awt::XWindow</code></li>
 *  <li> <code>com::sun::star::form::XBoundControl</code></li>
 *  <li> <code>com::sun::star::awt::XRadioButton</code></li>
 *  <li> <code>com::sun::star::awt::XControl</code></li>
 *  <li> <code>com::sun::star::awt::XLayoutConstrains</code></li>
 *  <li> <code>com::sun::star::awt::XView</code></li>
 * </ul> <p>
 * This object test <b> is NOT </b> designed to be run in several
 * threads concurently.
 *
 * @see com.sun.star.lang.XComponent
 * @see com.sun.star.awt.XWindow
 * @see com.sun.star.form.XBoundControl
 * @see com.sun.star.awt.XRadioButton
 * @see com.sun.star.awt.XControl
 * @see com.sun.star.awt.XLayoutConstrains
 * @see com.sun.star.awt.XView
 * @see ifc.lang._XComponent
 * @see ifc.awt._XWindow
 * @see ifc.form._XBoundControl
 * @see ifc.awt._XRadioButton
 * @see ifc.awt._XControl
 * @see ifc.awt._XLayoutConstrains
 * @see ifc.awt._XView
 */
public class ORadioButtonControl extends TestCase {

    XTextDocument xTextDoc;

    /**
     * Creates a new text document.
     */
    protected void initialize ( TestParameters Param, PrintWriter log) {
        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)Param.getMSF() );

        try {
            log.println( "creating a textdocument" );
            xTextDoc = SOF.createTextDoc( null );
        } catch ( com.sun.star.uno.Exception e ) {
            // Some exception occures.FAILED
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }
    }

    /**
     * Disposes the text document created before
     */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xTextDoc " );
        xTextDoc.dispose();
    }

    /**
     * Creates two components and inserts them to the form of
     * text document. One component
     * (<code>com.sun.star.form.component.RadioButton</code>) is created
     * for testing, another to be passed as relation. Using a controller
     * of the text document the controller of the first component is
     * obtained and returned in environment as a test object. <p>
     *
     *     Object relations created :
     * <ul>
     *  <li> <code>'GRAPHICS'</code> for
     *      {@link ifc.awt._XView} : a graphics component
     *      created using screen device of the window peer of
     *      the controller tested. </li>
     *  <li> <code>'CONTEXT'</code> for
     *      {@link ifc.awt._XControl} : the text document
     *      where the component is inserted. </li>
     *  <li> <code>'WINPEER'</code> for
     *      {@link ifc.awt._XControl} : Window peer of the
     *      controller tested. </li>
     *  <li> <code>'TOOLKIT'</code> for
     *      {@link ifc.awt._XControl} : toolkit of the component.</li>
     *  <li> <code>'MODEL'</code> for
     *      {@link ifc.awt._XControl} : the model of the controller.</li>
     *  <li> <code>'XWindow.AnotherWindow'</code> for
     *      {@link ifc.awt._XWindow} : the controller of another
     *      component. </li>
     * </ul>
     */
    public TestEnvironment createTestEnvironment( TestParameters Param,
                                                  PrintWriter log )
                                                    throws StatusException {
        XInterface oObj = null;
        XWindowPeer the_win = null;
        XToolkit the_kit = null;
        XDevice aDevice = null;
        XGraphics aGraphic = null;
        XControl aControl = null;
        //Insert a ControlShape and get the ControlModel
        XControlShape aShape = FormTools.createControlShape(
                                xTextDoc,3000,4500,15000,10000,"RadioButton");

        WriterTools.getDrawPage(xTextDoc).add((XShape) aShape);

        XControlModel the_Model = aShape.getControl();

        XControlShape aShape2 = FormTools.createControlShape(
                                xTextDoc,3000,4500,5000,10000,"TextField");

        WriterTools.getDrawPage(xTextDoc).add((XShape) aShape2);

        XControlModel the_Model2 = aShape2.getControl();

        //Try to query XControlAccess
        XControlAccess the_access = (XControlAccess) UnoRuntime.queryInterface(
                        XControlAccess.class,xTextDoc.getCurrentController());

        //now get the ORadioButtonControl
        try {
            oObj = the_access.getControl(the_Model);
            aControl = the_access.getControl(the_Model2);
            the_win = the_access.getControl(the_Model).getPeer();
            the_kit = the_win.getToolkit();
            aDevice = the_kit.createScreenCompatibleDevice(200,200);
            aGraphic = aDevice.createGraphics();
        } catch (com.sun.star.container.NoSuchElementException e) {
            log.println("Couldn't get ORadioButtonControl");
            e.printStackTrace(log);
            throw new StatusException("Couldn't get ORadioButtonControl", e );
        }

        log.println( "creating a new environment for ORadioButtonControl object" );
        TestEnvironment tEnv = new TestEnvironment( oObj );

        //Adding ObjRelation for XView
        tEnv.addObjRelation("GRAPHICS",aGraphic);

        //Adding ObjRelation for XControl
        tEnv.addObjRelation("CONTEXT",xTextDoc);
        tEnv.addObjRelation("WINPEER",the_win);
        tEnv.addObjRelation("TOOLKIT",the_kit);
        tEnv.addObjRelation("MODEL",the_Model);

        XWindow forObjRel = (XWindow)
                            UnoRuntime.queryInterface(XWindow.class,aControl);

        tEnv.addObjRelation("XWindow.AnotherWindow",forObjRel);

        return tEnv;
    } // finish method getTestEnvironment

}    // finish class ORadioButtonControl

