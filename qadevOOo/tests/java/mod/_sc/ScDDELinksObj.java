/*************************************************************************
 *
 *  $RCSfile: ScDDELinksObj.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change:$Date: 2003-01-27 18:16:34 $
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

package mod._sc;

import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XIndexAccess;
import com.sun.star.frame.XComponentLoader;
import com.sun.star.frame.XDesktop;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import java.io.PrintWriter;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.SOfficeFactory;
import util.utils;

/**
* Test for object which is represented by service
* <code>com.sun.star.sheet.DDELinks</code>. <p>
* Object implements the following interfaces :
* <ul>
*  <li> <code>com::sun::star::container::XNameAccess</code></li>
*  <li> <code>com::sun::star::container::XElementAccess</code></li>
* </ul>
* The following files used by this test :
* <ul>
*  <li><b> ScDDELinksObj.sdc </b> : the predefined testdocument </li>
* </ul> <p>
* @see com.sun.star.sheet.DDELinks
* @see com.sun.star.container.XNameAccess
* @see com.sun.star.container.XElementAccess
* @see ifc.container._XNameAccess
* @see ifc.container._XElementAccess
*/
public class ScDDELinksObj extends TestCase {
    XSpreadsheetDocument xSheetDoc = null;
    XComponent oDoc = null;

    /**
    * Creates Spreadsheet document.
    */
    protected void initialize( TestParameters tParam, PrintWriter log ) {
        SOfficeFactory SOF = SOfficeFactory.getFactory( tParam.getMSF() );

        try {
            log.println( "creating a Spreadsheet document" );
            xSheetDoc = SOF.createCalcDoc(null);
        } catch ( com.sun.star.uno.Exception e ) {
            // Some exception occures.FAILED
            e.printStackTrace( log );
            throw new StatusException( "Couldn�t create document", e );
        }

    }

    /**
    * Disposes Spreadsheet document and testdocument.
    */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xSheetDoc " );
        XComponent oComp = (XComponent) UnoRuntime.
                                queryInterface(XComponent.class, xSheetDoc) ;
        oComp.dispose();
        oDoc.dispose();
    }

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Disposes the testdocument if it was loaded already.
    * Creates an instance of the <code>com.sun.star.frame.Desktop</code>
    * and loads the predefined testdocument. Retrieves a collection of
    * spreadsheets from a document and takes one of them. Sets specific formula
    * to some cells in the spreadsheet(the formula specify DDE links to the
    * testdocument). Retrieves the collection of DDE links in the document.
    * The retrived collection of DDE link is the instance of the service
    * <code>com.sun.star.sheet.DDELinks</code>.
    * @see com.sun.star.frame.Desktop
    * @see com.sun.star.sheet.DDELinks
    */
    public synchronized TestEnvironment createTestEnvironment(
        TestParameters Param, PrintWriter log) throws StatusException {

        Object oInterface = null;
        XInterface oObj = null;

        // creation of testobject here
        // first we write what we are intend to do to log file
        log.println( "Creating a test environment" );

        // create testobject here

        XMultiServiceFactory oMSF = Param.getMSF();
        try {
            oInterface = oMSF.createInstance("com.sun.star.frame.Desktop" );
        } catch(com.sun.star.uno.Exception e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create instance", e);
        }

        // query the desktop interface and then it's componentloader
        XDesktop oDesktop = ( XDesktop )
            UnoRuntime.queryInterface(XDesktop.class, oInterface);
        XComponentLoader oCLoader = ( XComponentLoader )
            UnoRuntime.queryInterface(XComponentLoader.class, oDesktop);

        // load the predefined testdocument
        String testdoc = utils.getFullTestURL("ScDDELinksObj.sdc");
        try {
            oDoc = SOfficeFactory.getFactory(oMSF).loadDocument(testdoc);
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            e.printStackTrace(log);
            throw new StatusException("Can't load test document", e);
        } catch (com.sun.star.io.IOException e) {
            e.printStackTrace(log);
            throw new StatusException("Can't load test document", e);
        } catch (com.sun.star.uno.Exception e) {
            e.printStackTrace(log);
            throw new StatusException("Can't load test document", e);
        }

        log.println("getting sheets");
        XSpreadsheets xSpreadsheets = (XSpreadsheets)xSheetDoc.getSheets();

        log.println("getting a sheet");
        XSpreadsheet oSheet = null;
        XIndexAccess oIndexAccess = (XIndexAccess)
            UnoRuntime.queryInterface(XIndexAccess.class, xSpreadsheets);
        try {
            oSheet = (XSpreadsheet)oIndexAccess.getByIndex(0);
        } catch (com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't get a spreadsheet", e);
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't get a spreadsheet", e);
        }

        testdoc = utils.getFullTestDocName("ScDDELinksObj.sdc");
        log.println("filling some cells");
        try {
            oSheet.getCellByPosition(5, 5).setFormula(
                "=DDE(\"soffice\";\""+testdoc+"\";\"Sheet1.A1\"");
            oSheet.getCellByPosition(1, 4).setFormula(
                "=DDE(\"soffice\";\""+testdoc+"\";\"Sheet1.A1\"");
            oSheet.getCellByPosition(2, 0).setFormula(
                "=DDE(\"soffice\";\""+testdoc+"\";\"Sheet1.A1\"");
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            e.printStackTrace(log);
            throw new StatusException(
                "Exception occurred while filling cells", e);
        }

        try {
            log.println("Getting test object ") ;

            // Getting named ranges.
            XPropertySet docProps = (XPropertySet)
                UnoRuntime.queryInterface(XPropertySet.class, xSheetDoc);
            oObj = (XInterface)docProps.getPropertyValue("DDELinks");
            log.println("Creating object - " +
                                        ((oObj == null) ? "FAILED" : "OK"));
        } catch (com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log) ;
            throw new StatusException(
                "Error getting test object from spreadsheet document", e) ;
        } catch (com.sun.star.beans.UnknownPropertyException e) {
            e.printStackTrace(log) ;
            throw new StatusException(
                "Error getting test object from spreadsheet document", e) ;
        }

        TestEnvironment tEnv = new TestEnvironment( oObj );

        return tEnv;
    }

}


