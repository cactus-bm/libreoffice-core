/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ScCellFieldsObj.java,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:43:24 $
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
package mod._sc;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.SOfficeFactory;

import com.sun.star.container.XIndexAccess;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.table.XCell;
import com.sun.star.text.XText;
import com.sun.star.text.XTextContent;
import com.sun.star.text.XTextFieldsSupplier;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

/**
 * Test for object that represents a colection of text fields
 * in a cell of a spreadsheet. <p>
 *
 * Object implements the following interfaces :
 * <ul>
 *  <li> <code>com::sun::star::container::XEnumerationAccess</code></li>
 *  <li> <code>com::sun::star::util::XRefreshable</code></li>
 *  <li> <code>com::sun::star::container::XElementAccess</code></li>
 * </ul> <p>
 *
 * @see com.sun.star.container.XEnumerationAccess
 * @see com.sun.star.util.XRefreshable
 * @see com.sun.star.container.XElementAccess
 * @see ifc.container._XEnumerationAccess
 * @see ifc.util._XRefreshable
 * @see ifc.container._XElementAccess
 */
public class ScCellFieldsObj extends TestCase {
    XSpreadsheetDocument xSheetDoc = null;

    /**
    * Creates Spreadsheet document.
    */
    protected void initialize( TestParameters tParam, PrintWriter log ) {
        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF() );

        try {
            log.println( "creating a Spreadsheet document" );
            xSheetDoc = SOF.createCalcDoc(null);
        } catch ( com.sun.star.uno.Exception e ) {
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
        util.DesktopTools.closeDoc(oComp);
    }

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Creates an instance of the service
    * <code>com.sun.star.text.TextField.URL</code>, inserts it to the content
    * of the cell in the spreadsheet. Then the component is obtained
    * by <code>XTextFieldsSupplier</code> interface  of a cell.<p>
    */
    protected synchronized TestEnvironment createTestEnvironment(
        TestParameters Param, PrintWriter log) {

        XInterface oObj = null;
        XText oText = null;
        XTextContent oContent = null;
        XInterface aField = null;

        try {
            // we want to create an instance of ScCellFieldObj.
            // to do this we must get an MultiServiceFactory.

            XMultiServiceFactory _oMSF = (XMultiServiceFactory)
                UnoRuntime.queryInterface(XMultiServiceFactory.class, xSheetDoc);

            aField = (XInterface)
                _oMSF.createInstance("com.sun.star.text.TextField.URL");
            oContent = (XTextContent)
                UnoRuntime.queryInterface(XTextContent.class, aField);

            XSpreadsheets oSheets = xSheetDoc.getSheets() ;
            XIndexAccess oIndexSheets = (XIndexAccess)
                UnoRuntime.queryInterface(XIndexAccess.class, oSheets);
            XSpreadsheet oSheet = (XSpreadsheet) AnyConverter.toObject(
                        new Type(XSpreadsheet.class),oIndexSheets.getByIndex(0));

            XCell oCell = oSheet.getCellByPosition(2,3);
            oText = (XText)UnoRuntime.queryInterface(XText.class, oCell);

            oText.insertTextContent(
                oText.createTextCursor(), oContent, true);

            XTextFieldsSupplier xTextFieldsSupp = (XTextFieldsSupplier)
                UnoRuntime.queryInterface(XTextFieldsSupplier.class, oCell);

            oObj = xTextFieldsSupp.getTextFields();
        } catch (com.sun.star.lang.WrappedTargetException e) {
            log.println("Exception occured while creating test Object.");
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            log.println("Exception occured while creating test Object.");
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            log.println("Exception occured while creating test Object.");
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        } catch (com.sun.star.uno.Exception e) {
            log.println("Exception occured while creating test Object.");
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        }

        TestEnvironment tEnv = new TestEnvironment(oObj) ;

        return tEnv;
    }

}

