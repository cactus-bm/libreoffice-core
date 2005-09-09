/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: XMLExporter.java,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 03:06:22 $
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
import util.XMLTools;

import com.sun.star.container.XIndexAccess;
import com.sun.star.container.XNamed;
import com.sun.star.document.XExporter;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.uno.Any;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.xml.sax.XDocumentHandler;

/**
 * Test for object which is represented by service
 * <code>com.sun.star.comp.Calc.XMLExporter</code>. <p>
 * Object implements the following interfaces :
 * <ul>
 *  <li><code>com::sun::star::lang::XInitialization</code></li>
 *  <li><code>com::sun::star::document::ExportFilter</code></li>
 *  <li><code>com::sun::star::document::XFilter</code></li>
 *  <li><code>com::sun::star::document::XExporter</code></li>
 *  <li><code>com::sun::star::beans::XPropertySet</code></li>
 * </ul>
 * @see com.sun.star.lang.XInitialization
 * @see com.sun.star.document.ExportFilter
 * @see com.sun.star.document.XFilter
 * @see com.sun.star.document.XExporter
 * @see com.sun.star.beans.XPropertySet
 * @see ifc.lang._XInitialization
 * @see ifc.document._ExportFilter
 * @see ifc.document._XFilter
 * @see ifc.document._XExporter
 * @see ifc.beans._XPropertySet
 */
public class XMLExporter extends TestCase {
    XComponent xSheetDoc;

    /**
     * New spreadsheet document created.
     */
    protected void initialize( TestParameters tParam, PrintWriter log ) {
        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF() );
        try {
            log.println( "creating a calc document" );
            xSheetDoc = SOF.openDoc("scalc","_blank");
        } catch ( com.sun.star.uno.Exception e ) {
            // Some exception occures.FAILED
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }
    }

    /**
     * Spreadsheet document disposed
     */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xCalcDoc " );
        util.DesktopTools.closeDoc(xSheetDoc);
    }

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Creates an instance of the service
    * <code>com.sun.star.comp.Calc.XMLExporter</code> with
    * argument which is an implementation of <code>XDocumentHandler</code>
    * and which can check if required tags and character data is
    * exported. <p>
    * The calc document is set as a source document for exporter
    * created. Checking whether tags for table corresponding to some sheet
    * is exported.<p>
    *     Object relations created :
    * <ul>
    *  <li> <code>'MediaDescriptor'</code> for
    *      {@link ifc.document._XFilter} interface </li>
    *  <li> <code>'XFilter.Checker'</code> for
    *      {@link ifc.document._XFilter} interface </li>
    *  <li> <code>'SourceDocument'</code> for
    *      {@link ifc.document._XExporter} interface </li>
    * </ul>
    */
    protected synchronized TestEnvironment createTestEnvironment(TestParameters tParam, PrintWriter log) {

        XMultiServiceFactory xMSF = (XMultiServiceFactory)tParam.getMSF() ;
        XInterface oObj = null;
        FilterChecker Filter = new FilterChecker(log);
        Any arg = new Any(new Type(XDocumentHandler.class), Filter);
        final String SHEET_NAME = "XMLExporter_SheetTestName";

        try {
            oObj = (XInterface) xMSF.createInstanceWithArguments(
                "com.sun.star.comp.Calc.XMLExporter", new Object[] {arg} );
            XExporter xEx = (XExporter) UnoRuntime.queryInterface
                (XExporter.class,oObj);
            xEx.setSourceDocument(xSheetDoc);

            //set name of sheet
            XSpreadsheetDocument xSpreadsheetDoc = (XSpreadsheetDocument)
                UnoRuntime.queryInterface(XSpreadsheetDocument.class, xSheetDoc);
            XSpreadsheets xSpreadsheets = xSpreadsheetDoc.getSheets();
            XIndexAccess xSheetsIndexArray = (XIndexAccess)
                UnoRuntime.queryInterface(XIndexAccess.class, xSpreadsheets);
            XSpreadsheet xSheet = (XSpreadsheet) AnyConverter.toObject(
                    new Type(XSpreadsheet.class),xSheetsIndexArray.getByIndex(0));
            XNamed xSheetNamed = (XNamed)
                UnoRuntime.queryInterface(XNamed.class, xSheet);
            xSheetNamed.setName(SHEET_NAME);
        } catch (com.sun.star.uno.Exception e) {
            e.printStackTrace(log) ;
            throw new StatusException("Can't create component.", e) ;
        }

        // adding tags which must be contained in XML output
        Filter.addTag( new XMLTools.Tag("office:document") );
        Filter.addTagEnclosed(
            new XMLTools.Tag("office:meta"),
            new XMLTools.Tag("office:document") );
        Filter.addTagEnclosed(
            new XMLTools.Tag("office:settings"),
            new XMLTools.Tag("office:document") );
        Filter.addTagEnclosed(
            new XMLTools.Tag("office:script"),
            new XMLTools.Tag("office:document") );
        Filter.addTagEnclosed(
            new XMLTools.Tag("office:styles"),
            new XMLTools.Tag("office:document") );
        Filter.addTagEnclosed(
            new XMLTools.Tag("office:body"),
            new XMLTools.Tag("office:document") );
        Filter.addTagEnclosed(
            new XMLTools.Tag("table:table"),
            new XMLTools.Tag("office:body") );
        Filter.addTag(
            new XMLTools.Tag("table:table", "table:name", SHEET_NAME) );

        // create testobject here
        log.println( "creating a new environment" );
        TestEnvironment tEnv = new TestEnvironment( oObj );

        tEnv.addObjRelation("MediaDescriptor", XMLTools.createMediaDescriptor(
            new String[] {"FilterName"},
            new Object[] {"scalc: StarOffice XML (Calc)"}));
        tEnv.addObjRelation("SourceDocument",xSheetDoc);
        tEnv.addObjRelation("XFilter.Checker", Filter) ;
        return tEnv;
    }

    /**
     * This class checks the XML for tags and data required and returns
     * checking result to <code>XFilter</code> interface test. All
     * the information about errors occured in XML data is written
     * to log specified.
     * @see ifc.document._XFilter
     */
    protected class FilterChecker extends XMLTools.XMLChecker
        implements ifc.document._XFilter.FilterChecker {

        /**
         * Creates a class which will write information
         * into log specified.
         */
        public FilterChecker(PrintWriter log) {
            super(log, false) ;
        }
        /**
         * <code>_XFilter.FilterChecker</code> interface method
         * which returns the result of XML checking.
         * @return <code>true</code> if the XML data exported was
         * valid (i.e. all necessary tags and character data exists),
         * <code>false</code> if some errors occured.
         */
        public boolean checkFilter() {
            return check();
        }
    }
}

