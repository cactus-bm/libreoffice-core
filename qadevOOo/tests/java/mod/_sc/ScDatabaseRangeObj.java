/*************************************************************************
 *
 *  $RCSfile: ScDatabaseRangeObj.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change:$Date: 2003-05-27 13:05:12 $
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
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.container.XNameAccess;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XDatabaseRanges;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.table.CellRangeAddress;
import com.sun.star.table.XCellRange;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import java.io.PrintWriter;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.SOfficeFactory;

import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;

/**
* Test for object which is represented by service
* <code>com.sun.star.sheet.DatabaseRange</code>. <p>
* Object implements the following interfaces :
* <ul>
*  <li> <code>com::sun::star::container::XNamed</code></li>
*  <li> <code>com::sun::star::sheet::DatabaseRange</code></li>
*  <li> <code>com::sun::star::beans::XPropertySet</code></li>
*  <li> <code>com::sun::star::sheet::XDatabaseRange</code></li>
*  <li> <code>com::sun::star::sheet::XCellRangeReferrer</code></li>
* </ul>
* @see com.sun.star.sheet.DatabaseRange
* @see com.sun.star.container.XNamed
* @see com.sun.star.sheet.DatabaseRange
* @see com.sun.star.beans.XPropertySet
* @see com.sun.star.sheet.XDatabaseRange
* @see com.sun.star.sheet.XCellRangeReferrer
* @see ifc.container._XNamed
* @see ifc.sheet._DatabaseRange
* @see ifc.beans._XPropertySet
* @see ifc.sheet._XDatabaseRange
* @see ifc.sheet._XCellRangeReferrer
*/
public class ScDatabaseRangeObj extends TestCase {
        XSpreadsheetDocument xSheetDoc = null;

    /**
    * Creates Spreadsheet document.
    */
    protected void initialize( TestParameters tParam, PrintWriter log ) {
        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF() );

        try {
            log.println( "creating a Spreadsheet document" );
            xSheetDoc = SOF.createCalcDoc(null);
        } catch (com.sun.star.uno.Exception e) {
            // Some exception occures.FAILED
            e.printStackTrace( log );
            throw new StatusException( "Couldn�t create document", e );
        }
    }

    /**
    * Disposes Spreadsheet document.
    */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xSheetDoc " );
        XComponent oComp = (XComponent)
            UnoRuntime.queryInterface (XComponent.class, xSheetDoc) ;
        oComp.dispose();
    }

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Retrieves the collection of database ranges in the document.
    * If the database range with name <code>'dbRange'</code> exists
    * in the collection then removes it from the collection.
    * Creates new database range and adds it to the collection with the name
    * <code>'dbRange'</code>.The database range that was added to the collection
    * is the instance of the service <code>com.sun.star.sheet.DatabaseRange</code>.
    * Object relations created :
    * <ul>
    *  <li> <code>'DATAAREA'</code> for
    *      {@link ifc.sheet._XCellRangeReferrer}(of type
    *      <code>CellRangeAddress</code>)</li>
    *  <li> <code>'XCELLRANGE'</code> (of type <code>XCellRange</code>):
    *   cell range of the spreadsheet with database range</li>
    * </ul>
    * @see com.sun.star.sheet.DatabaseRange
    * @see com.sun.star.table.CellRangeAddress
    */
    protected synchronized TestEnvironment createTestEnvironment(TestParameters Param, PrintWriter log) {

        XInterface oObj = null;
        boolean result = true;

        // creation of testobject here
        // first we write what we are intend to do to log file
        log.println( "Creating a test environment" );

        log.println("Getting test object ") ;

        XPropertySet docProps = (XPropertySet)
            UnoRuntime.queryInterface(XPropertySet.class, xSheetDoc);

        XDatabaseRanges dbRanges = null;
        try {
            dbRanges = (XDatabaseRanges) AnyConverter.toObject(
                new Type(XDatabaseRanges.class),
                    docProps.getPropertyValue("DatabaseRanges"));
        } catch (com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't get a property", e);
        } catch (com.sun.star.beans.UnknownPropertyException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't get a property", e);
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't get a property", e);
        }

        if (dbRanges.hasByName("dbRange")) {
            dbRanges.removeByName("dbRange");
        }

        CellRangeAddress aRange = new CellRangeAddress((short)0, 2, 4, 5, 6);
        dbRanges.addNewByName("dbRange", aRange);

        XNameAccess dbrNA = (XNameAccess)
            UnoRuntime.queryInterface(XNameAccess.class, dbRanges);

        try {
            oObj = (XInterface) AnyConverter.toObject(
                        new Type(XInterface.class),dbrNA.getByName("dbRange"));
        } catch (com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log) ;
            throw new StatusException(
                "Error getting test object from spreadsheet document",e) ;
        } catch (com.sun.star.container.NoSuchElementException e) {
            e.printStackTrace(log) ;
            throw new StatusException(
                "Error getting test object from spreadsheet document",e) ;
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            e.printStackTrace(log) ;
            throw new StatusException(
                "Error getting test object from spreadsheet document",e) ;
        }

        TestEnvironment tEnv = new TestEnvironment( oObj );

        // Other parameters required for interface tests
        tEnv.addObjRelation("DATAAREA", aRange);

        XSpreadsheets sheets = xSheetDoc.getSheets();
        String names[] = sheets.getElementNames();
        XCellRange xCellRange = null;
        try {
            Object sheet = sheets.getByName(names[0]);
            xCellRange = (XCellRange)UnoRuntime.queryInterface(
                XCellRange.class, sheet);
        } catch(com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log);
            throw new StatusException(
                "Error getting of first spreadsheet from spreadsheet"
                + " document",e);
        } catch(com.sun.star.container.NoSuchElementException e) {
            e.printStackTrace(log);
            throw new StatusException(
                "Error getting of first spreadsheet from spreadsheet"
                + " document",e);
        }

        tEnv.addObjRelation("XCELLRANGE", xCellRange);

        return tEnv;
    }
}


