/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SwXAutoTextGroup.java,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 03:43:42 $
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

package mod._sw;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.SOfficeFactory;

import com.sun.star.container.XElementAccess;
import com.sun.star.container.XIndexAccess;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.text.XAutoTextContainer;
import com.sun.star.text.XText;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;


/**
 * Test for object which is represented by service
 * <code>com.sun.star.text.AutoTextGroup</code>. <p>
 * Object implements the following interfaces :
 * <ul>
 *  <li> <code>com::sun::star::container::XNamed</code></li>
 *  <li> <code>com::sun::star::container::XNameAccess</code></li>
 *  <li> <code>com::sun::star::container::XIndexAccess</code></li>
 *  <li> <code>com::sun::star::container::XElementAccess</code></li>
 *  <li> <code>com::sun::star::text::XAutoTextGroup</code></li>
 * </ul> <p>
 * This object test <b> is NOT </b> designed to be run in several
 * threads concurently.
 * @see com.sun.star.container.XNamed
 * @see com.sun.star.container.XNameAccess
 * @see com.sun.star.container.XIndexAccess
 * @see com.sun.star.container.XElementAccess
 * @see com.sun.star.text.XAutoTextGroup
 * @see com.sun.star.text.AutoTextContainer
 * @see ifc.container._XNamed
 * @see ifc.container._XNameAccess
 * @see ifc.container._XIndexAccess
 * @see ifc.container._XElementAccess
 * @see ifc.text._XAutoTextGroup
 */
public class SwXAutoTextGroup extends TestCase {
    XTextDocument xTextDoc;

    /**
    * Creates text document.
    */
    protected void initialize( TestParameters tParam, PrintWriter log ) {
        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF() );
        try {
            log.println( "creating a textdocument" );
            xTextDoc = SOF.createTextDoc( null );
        } catch ( com.sun.star.uno.Exception e ) {
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }
    }

    /**
    * Disposes text document.
    */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xTextDoc " );
        util.DesktopTools.closeDoc(xTextDoc);
    }

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Creates an instance of the service
    * <code>com.sun.star.text.AutoTextContainer</code>, then selects
    * non-empty group from the given container.<p>
    *     Object relations created :
    * <ul>
    *  <li> <code>'TextRange'</code> for
    *      {@link ifc.text._XAutoTextGroup} range of text</li>
    * </ul>
    */
    protected synchronized TestEnvironment createTestEnvironment(TestParameters Param, PrintWriter log) {

        XInterface oObj = null;
        XAutoTextContainer oContainer;

        log.println( "creating a test environment" );
        try {
            XMultiServiceFactory myMSF = (XMultiServiceFactory)Param.getMSF();
            Object oInst = myMSF.createInstance
                ("com.sun.star.text.AutoTextContainer");
            oContainer = (XAutoTextContainer)
                UnoRuntime.queryInterface(XAutoTextContainer.class,oInst);
        } catch (com.sun.star.uno.Exception e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create AutoTextContainer", e);
        }

        XIndexAccess oContIndex = (XIndexAccess)
            UnoRuntime.queryInterface(XIndexAccess.class, oContainer);

        //search for a non-empty AutoTextGroup
        try {
            int n = 0;
            int count = oContIndex.getCount();
            oObj = (XInterface) AnyConverter.toObject(
                    new Type(XInterface.class),oContIndex.getByIndex(n));
            XElementAccess oElement = (XElementAccess)
                UnoRuntime.queryInterface(XElementAccess.class, oObj);
            while ( ( !oElement.hasElements() ) && ( n < count ) ) {
                n++;
                oObj = (XInterface) AnyConverter.toObject(
                    new Type(XInterface.class),oContIndex.getByIndex(n));
                oElement = (XElementAccess)
                    UnoRuntime.queryInterface(XElementAccess.class, oObj);
            }
        } catch ( com.sun.star.lang.WrappedTargetException e ) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't get AutoTextGroup", e);
        } catch ( com.sun.star.lang.IndexOutOfBoundsException e ) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't get AutoTextGroup", e);
        } catch ( com.sun.star.lang.IllegalArgumentException e ) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't get AutoTextGroup", e);
        }

        log.println( "creating a new environment for AutoTextGroup object" );
        TestEnvironment tEnv = new TestEnvironment( oObj );

        XText oText = xTextDoc.getText();
        oText.insertString(oText.getStart(), "New AutoText", true);

        log.println( "adding TextRange as mod relation to environment" );
        tEnv.addObjRelation("TextRange", oText);

        return tEnv;
    } // finish method getTestEnvironment


}    // finish class SwXAutoTextGroup
