/*************************************************************************
 *
 *  $RCSfile: XMLSettingsImporter.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change:$Date: 2003-05-27 14:13:13 $
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

package mod._xmloff.Draw;

import com.sun.star.beans.XPropertySet;
import com.sun.star.document.XImporter;
import com.sun.star.frame.XController;
import com.sun.star.frame.XModel;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import java.io.PrintWriter;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.SOfficeFactory;

/**
 * Test for object which is represented by service
 * <code>com.sun.star.comp.Draw.XMLSettingsImporter</code>. <p>
 * Object implements the following interfaces :
 * <ul>
 *  <li><code>com::sun::star::lang::XInitialization</code></li>
 *  <li><code>com::sun::star::document::XImporter</code></li>
 *  <li><code>com::sun::star::document::XFilter</code></li>
 *  <li><code>com::sun::star::document::ImportFilter</code></li>
 *  <li><code>com::sun::star::beans::XPropertySet</code></li>
 *  <li><code>com::sun::star::xml::sax::XDocumentHandler</code></li>
 * </ul>
 * @see com.sun.star.lang.XInitialization
 * @see com.sun.star.document.XImporter
 * @see com.sun.star.document.XFilter
 * @see com.sun.star.document.ImportFilter
 * @see com.sun.star.beans.XPropertySet
 * @see com.sun.star.xml.sax.XDocumentHandler
 * @see ifc.lang._XInitialization
 * @see ifc.document._XImporter
 * @see ifc.document._XFilter
 * @see ifc.document._XExporter
 * @see ifc.beans._XPropertySet
 * @see ifc.xml.sax._XDocumentHandler
 */
public class XMLSettingsImporter extends TestCase {
    XComponent xDrawDoc = null;

    /**
    * New draw document created.
    */
    protected void initialize( TestParameters tParam, PrintWriter log ) {

        // get a soffice factory object
        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF());

        try {
            log.println( "creating a drawdocument" );
            xDrawDoc = SOF.createDrawDoc(null);
        } catch ( Exception e ) {
            // Some exception occures.FAILED
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }
    }

    /**
    * Text draw destroyed.
    */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing document " );
        xDrawDoc.dispose();
    }

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Creates an instance of the service
    * <code>com.sun.star.comp.Draw.XMLSettingsImporter</code><p>
    *
    * The draw document is set as a target document for importer.
    * Imported XML-data contains only settings tags including
    * tag with property 'IsLayerMode'.
    * After import property 'IsLayerMode' getting from target
    * document is checked.
    *     Object relations created :
    * <ul>
    *  <li> <code>'XDocumentHandler.XMLData'</code> for
    *      {@link ifc.xml.sax._XDocumentHandler} interface </li>
    *  <li> <code>'XDocumentHandler.ImportChecker'</code> for
    *      {@link ifc.xml.sax._XDocumentHandler} interface </li>
    *  <li> <code>'TargetDocument'</code> for
    *      {@link ifc.document._XImporter} interface </li>
    * </ul>
    */
    public synchronized TestEnvironment createTestEnvironment
            (TestParameters tParam, PrintWriter log) throws StatusException {

        XInterface oObj = null;
        Object oInt = null ;

        // creation of testobject here
        // first we write what we are intend to do to log file
        log.println( "creating a test environment" );

        XMultiServiceFactory xMSF = (XMultiServiceFactory)tParam.getMSF() ;

        try {
            oInt = xMSF.createInstance
                ("com.sun.star.comp.Draw.XMLSettingsImporter") ;
            XImporter imp = (XImporter)
                UnoRuntime.queryInterface(XImporter.class, oInt) ;
            //imp.setTargetDocument(xDrawDoc);
        } catch (com.sun.star.uno.Exception e) {
            e.printStackTrace(log) ;
            throw new StatusException("Can't create component.", e) ;
        }

        oObj = (XInterface) oInt ;

        // create testobject here
        log.println( "creating a new environment for Paragraph object" );
        TestEnvironment tEnv = new TestEnvironment( oObj );

        // adding relation
        tEnv.addObjRelation("TargetDocument", xDrawDoc) ;

        // adding relation for XDocumentHandler
        String[][] xml = new String[][] {
            {"start", "office:document-settings",
                "xmlns:office", "CDATA", "http://openoffice.org/2000/office",
                "xmlns:config", "CDATA", "http://openoffice.org/2001/config"},
                {"start", "office:settings"},
                    {"start", "config:config-item-set",
                        "config:name", "CDATA", "view-settings"},
                        {"start", "config:config-item-map-indexed",
                            "config:name", "CDATA", "Views"},
                            {"start", "config:config-item-map-entry"},
                                {"start", "config:config-item",
                                    "config:name", "CDATA", "IsLayerMode",
                                    "config:type", "CDATA", "boolean"},
                                {"chars", "true"},
                                {"end", "config:config-item"},
                            {"end", "config:config-item-map-entry"},
                        {"end", "config:config-item-map-indexed"},
                    {"end", "config:config-item-set"},
                {"end", "office:settings"},
            {"end", "office:document-settings"} };

        tEnv.addObjRelation("XDocumentHandler.XMLData", xml);
        final PrintWriter logF = log;
        XModel xDrawModel = (XModel)
            UnoRuntime.queryInterface(XModel.class, xDrawDoc);
        XController xController = xDrawModel.getCurrentController();
        final XPropertySet xPropSet = (XPropertySet)
            UnoRuntime.queryInterface(XPropertySet.class, xController);

        tEnv.addObjRelation("XDocumentHandler.ImportChecker",
            new ifc.xml.sax._XDocumentHandler.ImportChecker() {
                public boolean checkImport() {
                    try {
                        Boolean value = (Boolean)
                            xPropSet.getPropertyValue("IsLayerMode");
                        logF.println("'IsLayerMode' property value is '"
                            + value + "'");
                        return value.booleanValue() == true;
                    } catch (com.sun.star.uno.Exception e) {
                        logF.println("Exception while checking import :") ;
                        e.printStackTrace(logF) ;
                        return false ;
                    }
                }
            }) ;

        return tEnv;
    } // finish method getTestEnvironment
}
