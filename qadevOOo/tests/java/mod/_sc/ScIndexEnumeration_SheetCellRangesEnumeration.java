/*************************************************************************
 *
 *  $RCSfile: ScIndexEnumeration_SheetCellRangesEnumeration.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change:$Date: 2003-02-03 13:30:33 $
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
import com.sun.star.container.XEnumerationAccess;
import com.sun.star.container.XIndexAccess;
import com.sun.star.container.XNameContainer;
import com.sun.star.lang.XComponent;
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

import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;

public class ScIndexEnumeration_SheetCellRangesEnumeration extends TestCase {
    XSpreadsheetDocument xSheetDoc = null;

    /**
    * Creates Spreadsheet document.
    */
    protected void initialize( TestParameters tParam, PrintWriter log ) {

        // get a soffice factory object
        SOfficeFactory SOF = SOfficeFactory.getFactory( tParam.getMSF());

        try {
            log.println( "creating a sheetdocument" );
            xSheetDoc = SOF.createCalcDoc(null);;
        } catch (com.sun.star.uno.Exception e) {
            // Some exception occures.FAILED
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }
    }

    /**
    * Disposes Spreadsheet document.
    */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xSheetDoc " );
        XComponent oComp =
            (XComponent) UnoRuntime.queryInterface (XComponent.class, xSheetDoc);
        oComp.dispose();
    }

    protected synchronized TestEnvironment createTestEnvironment(TestParameters Param, PrintWriter log) {

        XInterface oObj = null;
        Object oRange = null ;

        // creation of testobject here
        // first we write what we are intend to do to log file
        log.println( "Creating a test environment" );

        // get a soffice factory object
        SOfficeFactory SOF = SOfficeFactory.getFactory( Param.getMSF() );

        log.println("Getting test object ");

        XComponent oComp = (XComponent)
                    UnoRuntime.queryInterface (XComponent.class, xSheetDoc);

        oObj = (XInterface)
            SOF.createInstance(oComp, "com.sun.star.sheet.SheetCellRanges");

        XSpreadsheets oSheets = xSheetDoc.getSheets() ;
        XIndexAccess oIndSheets = (XIndexAccess)
            UnoRuntime.queryInterface (XIndexAccess.class, oSheets);
        XSpreadsheet oSheet = null;
        try {
            oSheet = (XSpreadsheet) AnyConverter.toObject(
                    new Type(XSpreadsheet.class),oIndSheets.getByIndex(0));
            XNameContainer oRanges = (XNameContainer)
                UnoRuntime.queryInterface(XNameContainer.class, oObj);

            oRange = oSheet.getCellRangeByName("C1:D4");
            oRanges.insertByName("Range1", oRange);
            oRange = oSheet.getCellRangeByName("E2:F5");
            oRanges.insertByName("Range2", oRange);
            oRange = oSheet.getCellRangeByName("G2:H3");
            oRanges.insertByName("Range3", oRange);
            oRange = oSheet.getCellRangeByName("I7:J8");
            oRanges.insertByName("Range4", oRange);
        } catch(com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        } catch(com.sun.star.lang.IndexOutOfBoundsException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        } catch(com.sun.star.container.ElementExistException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        } catch(com.sun.star.lang.IllegalArgumentException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        }

        log.println("filling some cells");
        try {
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 5; j++) {
                    oSheet.getCellByPosition(i, j).setFormula("a");
                }
            }
            for (int i = 0; i < 10; i++) {
                for (int j = 5; j < 10; j++) {
                    oSheet.getCellByPosition(i, j).setValue(i + j);
                }
            }
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            e.printStackTrace(log);
            throw new StatusException (
                "Exception occurred while filling cells", e);
        }

        XEnumerationAccess ea = (XEnumerationAccess)
                    UnoRuntime.queryInterface(XEnumerationAccess.class,oObj);

        oObj = ea.createEnumeration();

        log.println("ImplementationName: "+util.utils.getImplName(oObj));
        // creating test environment
        TestEnvironment tEnv = new TestEnvironment( oObj );

        tEnv.addObjRelation("ENUM",ea);

        return tEnv ;
    }

}

