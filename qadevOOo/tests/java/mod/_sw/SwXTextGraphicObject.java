/*************************************************************************
 *
 *  $RCSfile: SwXTextGraphicObject.java,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change:$Date: 2004-12-10 17:04:52 $
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

package mod._sw;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.SOfficeFactory;

import com.sun.star.beans.XPropertySet;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.text.TextContentAnchorType;
import com.sun.star.text.XText;
import com.sun.star.text.XTextContent;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

public class SwXTextGraphicObject extends TestCase {

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
        log.println( "    disposing xDrawDoc " );
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
    protected TestEnvironment createTestEnvironment
                (TestParameters tParam, PrintWriter log) {

            XInterface oObj = null;
            Object oGObject = null;
            Object xTextFrame = null;
            SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF() );

            Object instance = null;

            try {
                oGObject = SOF.createInstance
                    (xTextDoc,"com.sun.star.text.GraphicObject");
                instance = SOF.createInstance
                    (xTextDoc,"com.sun.star.text.GraphicObject");
                xTextFrame = SOF.createTextFrame(xTextDoc, 500, 500);
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

            log.println("inserting Frame");
            try{
                XTextContent Framecontent = (XTextContent) UnoRuntime.queryInterface(
                                                   XTextContent.class, xTextFrame);
                the_text.insertTextContent(the_cursor, Framecontent, true);
            } catch (Exception e) {
                System.out.println("Couldn't insert text frame");
                e.printStackTrace();
                throw new StatusException("Couldn't insert text frame", e );
            }


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
                oProps.setPropertyValue("AnchorType",
                    TextContentAnchorType.AT_PARAGRAPH);
                oProps.setPropertyValue("GraphicURL",wat);
                oProps.setPropertyValue("HoriOrientPosition",new Integer(5500));
                oProps.setPropertyValue("VertOrientPosition",new Integer(4200));
                oProps.setPropertyValue("Width",new Integer(4400));
                oProps.setPropertyValue("Height",new Integer(4000));
            } catch (Exception e) {
                System.out.println("Couldn't set property 'GraphicURL'");
                e.printStackTrace();
                throw new StatusException
                    ("Couldn't set property 'GraphicURL'", e );
            }

                    TestEnvironment tEnv = new TestEnvironment( oObj );

                    log.println( "adding ObjRelation for XShape "
                        +"(get/setPosition won't work there)" );
                    tEnv.addObjRelation("NoPos", "SwXTextGraphicObject");
                    tEnv.addObjRelation("NoSetSize","SwXTextGraphicObject");
            Object ImgMap = null;
            //creating ObjectRelation for the property
            // 'ImageMap' of 'TextGraphicObject'
            try {
                XMultiServiceFactory xDocMSF = (XMultiServiceFactory)
                    UnoRuntime.queryInterface
                    (XMultiServiceFactory.class,xTextDoc);
                ImgMap = xDocMSF.createInstance
                    ("com.sun.star.image.ImageMapRectangleObject");
            }
            catch (Exception ex) {
                ex.printStackTrace();
            }

            tEnv.addObjRelation("IMGMAP",ImgMap);

            tEnv.addObjRelation("CONTENT", (XTextContent)
                        UnoRuntime.queryInterface(XTextContent.class,instance));
            tEnv.addObjRelation("RANGE", xTextDoc.getText().createTextCursor());

            //object relation for text.BaseFrameProperties
            tEnv.addObjRelation("TextFrame", xTextFrame);

            return tEnv;

    } // finish method getTestEnvironment

}    // finish class SwXTextGraphicObject

