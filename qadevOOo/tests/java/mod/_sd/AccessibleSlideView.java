/*************************************************************************
 *
 *  $RCSfile: AccessibleSlideView.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change:$Date: 2003-04-28 12:30:40 $
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

package mod._sd;

import com.sun.star.awt.XWindow;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XDrawPages;
import com.sun.star.drawing.XDrawPagesSupplier;
import com.sun.star.drawing.XDrawView;
import com.sun.star.frame.XModel;
import com.sun.star.lang.XComponent;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.accessibility.AccessibleRole;
import com.sun.star.accessibility.XAccessible;
import java.io.PrintWriter;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.AccessibilityTools;
import util.SOfficeFactory;
import util.utils;
import com.sun.star.frame.XDispatch;
import com.sun.star.frame.XDispatchProvider;
import com.sun.star.util.URL;
import com.sun.star.util.XURLTransformer;

public class AccessibleSlideView extends TestCase {

    XModel aModel = null;
    XComponent xImpressDoc = null;

    /**
     * Called to create an instance of <code>TestEnvironment</code> with an
     * object to test and related objects. Subclasses should implement this
     * method to provide the implementation and related objects. The method is
     * called from <code>getTestEnvironment()</code>.
     *
     * @param tParam test parameters
     * @param log writer to log information while testing
     *
     * @see TestEnvironment
     * @see #getTestEnvironment()
     */
    protected TestEnvironment createTestEnvironment
            (TestParameters Param, PrintWriter log) {
        XInterface oObj = null;

        AccessibilityTools at = new AccessibilityTools();

        XDrawPagesSupplier oDPS = (XDrawPagesSupplier)
            UnoRuntime.queryInterface(XDrawPagesSupplier.class, aModel);
        XDrawPages oDPn = oDPS.getDrawPages();
        XDrawPage fDP1 = oDPn.insertNewByIndex(1);
        XDrawPage fDP2 = oDPn.insertNewByIndex(2);

        shortWait();

        XWindow xWindow = at.getCurrentWindow (Param.getMSF(),aModel);
        XAccessible xRoot = at.getAccessibleObject(xWindow);

        //at.printAccessibleTree(log, xRoot);

        at.getAccessibleObjectForRole(xRoot, AccessibleRole.DOCUMENT);

        oObj = at.SearchedContext;

        System.out.println("ImplementationName "+utils.getImplName(oObj));

        TestEnvironment tEnv = new TestEnvironment(oObj);

        //util.dbg.printInterfaces(oObj);
        log.println("Implementationname: "+util.utils.getImplName(oObj));

        final XDrawPages DrawPages = oDPn;

        tEnv.addObjRelation("EventProducer",
            new ifc.accessibility._XAccessibleEventBroadcaster.EventProducer() {
                public void fireEvent() {
                    DrawPages.insertNewByIndex(2);
                }
            });

        return tEnv;

    }

    /**
    * Called while disposing a <code>TestEnvironment</code>.
    * Disposes Impress documents.
    * @param tParam test parameters
    * @param tEnv the environment to cleanup
    * @param log writer to log information while testing
    */
    protected void cleanup( TestParameters Param, PrintWriter log ) {
        log.println("disposing impress document");
        xImpressDoc.dispose();
    }

    /**
     * Called while the <code>TestCase</code> initialization. In the
     * implementation does nothing. Subclasses can override to initialize
     * objects shared among all <code>TestEnvironment</code>s.
     *
     * @param tParam test parameters
     * @param log writer to log information while testing
     *
     * @see #initializeTestCase()
     */
    protected void initialize(TestParameters Param, PrintWriter log) {
        // get a soffice factory object
        SOfficeFactory SOF = SOfficeFactory.getFactory( Param.getMSF());

        try {
            log.println( "creating a impress document" );
            xImpressDoc = SOF.createImpressDoc(null);
        } catch (com.sun.star.uno.Exception e) {
            e.printStackTrace( log );
            throw new StatusException("Couldn't create document", e);
        }

        shortWait();

        aModel = (XModel)
            UnoRuntime.queryInterface(XModel.class, xImpressDoc);

        //Change to Outline view
        try {
            String aSlotID = "slot:27011";
            XDispatchProvider xDispProv = (XDispatchProvider)
                UnoRuntime.queryInterface( XDispatchProvider.class, aModel.getCurrentController() );
            XURLTransformer xParser = (com.sun.star.util.XURLTransformer)
                UnoRuntime.queryInterface(XURLTransformer.class,
        Param.getMSF().createInstance("com.sun.star.util.URLTransformer"));
            // Because it's an in/out parameter we must use an array of URL objects.
            URL[] aParseURL = new URL[1];
            aParseURL[0] = new URL();
            aParseURL[0].Complete = aSlotID;
            xParser.parseStrict(aParseURL);
            URL aURL = aParseURL[0];
            XDispatch xDispatcher = xDispProv.queryDispatch( aURL,"",0);
            if( xDispatcher != null )
                    xDispatcher.dispatch( aURL, null );
        } catch (com.sun.star.uno.Exception e) {
            log.println("Couldn't change mode");
        }

        shortWait();

    }

    private void shortWait() {
        try {
            Thread.sleep(2000) ;
        } catch (InterruptedException e) {
            System.out.println("While waiting :" + e) ;
        }
    }
}

