/*************************************************************************
 *
 *  $RCSfile: ScFilterDescriptorBase.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change:$Date: 2003-01-27 18:16:30 $
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

import com.sun.star.container.XIndexAccess;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSheetFilterable;
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

/**
* Test for object which is represented by service
* <code>com.sun.star.sheet.SheetFilterDescriptor</code>. <p>
* Object implements the following interfaces :
* <ul>
*  <li> <code>com::sun::star::sheet::XSheetFilterDescriptor</code></li>
*  <li> <code>com::sun::star::sheet::SheetFilterDescriptor</code></li>
*  <li> <code>com::sun::star::beans::XPropertySet</code></li>
* </ul>
* @see com.sun.star.sheet.SheetFilterDescriptor
* @see com.sun.star.sheet.XSheetFilterDescriptor
* @see com.sun.star.sheet.SheetFilterDescriptor
* @see com.sun.star.beans.XPropertySet
* @see ifc.sheet._XSheetFilterDescriptor
* @see ifc.sheet._SheetFilterDescriptor
* @see ifc.beans._XPropertySet
*/
public class ScFilterDescriptorBase extends TestCase {
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
        XComponent oComp = (XComponent)
            UnoRuntime.queryInterface (XComponent.class, xSheetDoc);
        oComp.dispose();
    }

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Retrieves a collection of spreadsheets from a document
    * and takes one of them. Fills some cells in the spreadsheet.
    * Creates the filter descriptor using the interface
    * <code>XSheetFilterable</code>. This filter descriptor is the instance
    * of the service <code>com.sun.star.sheet.SheetFilterDescriptor</code>.
    * @see com.sun.star.sheet.XSheetFilterable
    * @see com.sun.star.sheet.SheetFilterDescriptor
    */
    public synchronized TestEnvironment createTestEnvironment(
        TestParameters Param, PrintWriter log) throws StatusException {

        XInterface oObj = null;
        XSheetFilterable SF = null;

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

        log.println("filling some cells");
        try {
            oSheet.getCellByPosition(5, 5).setValue(15);
            oSheet.getCellByPosition(1, 4).setValue(10);
            oSheet.getCellByPosition(2, 0).setValue(-5.15);
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            e.printStackTrace(log);
            throw new StatusException(
                "Exception occurred while filling cells", e);
        }

        SF = (XSheetFilterable)
            UnoRuntime.queryInterface(XSheetFilterable.class, oSheet);

        oObj = SF.createFilterDescriptor(true);

        log.println("creating a new environment for object");
        TestEnvironment tEnv = new TestEnvironment(oObj);

        return tEnv;

    } // finish method getTestEnvironment

}    // finish class ScFilterDescriptorBase

