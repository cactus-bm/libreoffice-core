/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SwXTextGraphicObjects.java,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 03:58:23 $
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

import com.sun.star.beans.XPropertySet;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.text.XText;
import com.sun.star.text.XTextContent;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextGraphicObjectsSupplier;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

public class SwXTextGraphicObjects extends TestCase {

    XTextDocument xTextDoc;

    /**
     * in general this method creates a testdocument
     *
     *  @param tParam    class which contains additional test parameters
     *  @param log        class to log the test state and result
     *
     *
     *  @see TestParameters
     *    @see PrintWriter
     *
     */
    protected void initialize( TestParameters tParam, PrintWriter log ) {
        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF() );
        try {
            log.println( "creating a textdoc" );
            xTextDoc = SOF.createTextDoc( null );;
        } catch ( Exception e ) {
            // Some exception occures.FAILED
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }
    }

    /**
     * in general this method disposes the testenvironment and document
     *
     *  @param tParam    class which contains additional test parameters
     *  @param log        class to log the test state and result
     *
     *
     *  @see TestParameters
     *    @see PrintWriter
     *
     */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xTextDoc " );
        util.DesktopTools.closeDoc(xTextDoc);
    }


    /**
     *    creating a Testenvironment for the interfaces to be tested
     *
     *  @param tParam    class which contains additional test parameters
     *  @param log        class to log the test state and result
     *
     *  @return    Status class
     *
     *  @see TestParameters
     *    @see PrintWriter
     */
    public TestEnvironment createTestEnvironment( TestParameters tParam,
                                                  PrintWriter log )
                                                    throws StatusException {

        XInterface oObj = null;
        Object oGObject = null;
        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF() );

        try {
            oGObject = SOF.createInstance
                (xTextDoc,"com.sun.star.text.GraphicObject");
        }
        catch (Exception ex) {
            log.println("Couldn't create instance");
            ex.printStackTrace(log);
            throw new StatusException("Couldn't create instance", ex );
        }

        oObj = (XInterface) oGObject;

        XText the_text = xTextDoc.getText();
        XTextCursor the_cursor = the_text.createTextCursor();
        XTextContent the_content = (XTextContent)
                            UnoRuntime.queryInterface(XTextContent.class,oObj);

       log.println( "inserting graphic" );
        try {
            the_text.insertTextContent(the_cursor,the_content,true);
        } catch (Exception e) {
            System.out.println("Couldn't insert Content");
            e.printStackTrace();
            throw new StatusException("Couldn't insert Content", e );
        }

        log.println( "adding graphic" );
        XPropertySet oProps = (XPropertySet)
                            UnoRuntime.queryInterface(XPropertySet.class,oObj);
        try {
            String wat = util.utils.getFullTestURL("space-metal.jpg");
            oProps.setPropertyValue("GraphicURL",wat);
            oProps.setPropertyValue("HoriOrientPosition",new Integer(5500));
            oProps.setPropertyValue("VertOrientPosition",new Integer(4200));
            oProps.setPropertyValue("Width",new Integer(4400));
            oProps.setPropertyValue("Height",new Integer(4000));
        } catch (Exception e) {
            System.out.println("Couldn't set property 'GraphicURL'");
            e.printStackTrace();
            throw new StatusException("Couldn't set property 'GraphicURL'", e );
        }

        XTextGraphicObjectsSupplier xTGS = (XTextGraphicObjectsSupplier)
            UnoRuntime.queryInterface(XTextGraphicObjectsSupplier.class,
            xTextDoc);
        oObj = xTGS.getGraphicObjects();

        TestEnvironment tEnv = new TestEnvironment( oObj );

        return tEnv;

    } // finish method getTestEnvironment

}    // finish class SwXTextGraphicObjects

