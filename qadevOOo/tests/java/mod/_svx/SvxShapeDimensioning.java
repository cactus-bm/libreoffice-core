/*************************************************************************
 *
 *  $RCSfile: SvxShapeDimensioning.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change:$Date: 2003-05-27 13:37:31 $
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

package mod._svx;

import com.sun.star.beans.XPropertySet;
import com.sun.star.drawing.XShape;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.style.XStyle;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import java.io.PrintWriter;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.DefaultDsc;
import util.DrawTools;
import util.InstCreator;
import util.SOfficeFactory;

import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;

public class SvxShapeDimensioning extends TestCase {

    XComponent xDrawDoc;

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

        try {
            log.println( "creating a drawdoc" );
            xDrawDoc = DrawTools.createDrawDoc((XMultiServiceFactory)tParam.getMSF());
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
        xDrawDoc.dispose();
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
    protected TestEnvironment createTestEnvironment(TestParameters tParam, PrintWriter log) {

        XInterface oObj = null;
        XShape oShape = null;

        // creation of testobject here
        // first we write what we are intend to do to log file
        log.println( "creating a test environment" );

        try {

            SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF());
              oShape = SOF.createShape(xDrawDoc,4000,4000,3000,3000,"Measure");

              XPropertySet xProps = (XPropertySet) UnoRuntime.queryInterface(
                  XPropertySet.class, oShape) ;

              //xProps.setPropertyValue("LineStart",
              //  new Point[] {new Point(3000, 3000)}) ;
              //xProps.setPropertyValue("LineEnd",
              //  new Point[] {new Point(5000, 6000)}) ;

            DrawTools.getShapes(DrawTools.getDrawPage(xDrawDoc,0)).add(oShape);

            for (int i=0;i<10;i++) {
                DrawTools.getShapes(DrawTools.getDrawPage(xDrawDoc,0)).add(
                    SOF.createShape(xDrawDoc,
                    3000,4500,7510+10*i,5010+10*i,"Rectangle"));
            }

            oObj = oShape ;

            //SOfficeFactory SOF = SOfficeFactory.getFactory((XMultiServiceFactory)tParam.getMSF()) ;
            oShape = SOF.createShape(xDrawDoc,5000,3500,7500,5000,"Line");
            DrawTools.getShapes(DrawTools.getDrawPage(xDrawDoc,0)).add(oShape) ;
        }
        catch (Exception e) {
            log.println("Couldn't create insance");
            e.printStackTrace(log);
        }

        // test environment creation

         TestEnvironment tEnv = new TestEnvironment(oObj);

        log.println( "adding two styles as ObjRelation for ShapeDescriptor" );
        XPropertySet oShapeProps = (XPropertySet)
                            UnoRuntime.queryInterface(XPropertySet.class,oObj);
        XStyle aStyle = null;
        try {
            aStyle = (XStyle) AnyConverter.toObject(
                new Type(XStyle.class),oShapeProps.getPropertyValue("Style"));
        } catch (Exception e) {}
        tEnv.addObjRelation("Style1",aStyle);
        oShapeProps = (XPropertySet)
            UnoRuntime.queryInterface(XPropertySet.class,oShape);
        try {
            aStyle = (XStyle) AnyConverter.toObject(
                new Type(XStyle.class),oShapeProps.getPropertyValue("Style"));
        } catch (Exception e) {}
        tEnv.addObjRelation("Style2",aStyle);

        DefaultDsc tDsc = new DefaultDsc("com.sun.star.text.XTextContent",
                                            "com.sun.star.text.TextField.URL");
        log.println( "    adding InstCreator object" );
        tEnv.addObjRelation( "XTEXTINFO", new InstCreator( xDrawDoc, tDsc ) );

        return tEnv;
    } // finish method getTestEnvironment

}    // finish class SvxShapeDimensioning

