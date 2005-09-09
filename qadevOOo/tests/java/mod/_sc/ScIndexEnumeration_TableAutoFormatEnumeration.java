/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ScIndexEnumeration_TableAutoFormatEnumeration.java,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:56:42 $
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

import com.sun.star.container.XEnumerationAccess;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

public class ScIndexEnumeration_TableAutoFormatEnumeration extends TestCase{
    XSpreadsheetDocument xSheetDoc = null;
    SOfficeFactory SOF = null;

    /**
    * Creates Spreadsheet document.
    */
    protected void initialize( TestParameters tParam, PrintWriter log ) {
        SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF() );

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
        XComponent oComp = (XComponent) UnoRuntime.queryInterface
            (XComponent.class, xSheetDoc) ;
        util.DesktopTools.closeDoc(oComp);
    }

    protected synchronized TestEnvironment createTestEnvironment(TestParameters Param, PrintWriter log) {

        XInterface oObj = null;

        try {
            // creation of testobject here
            // get AutoFormats
            oObj = (XInterface)((XMultiServiceFactory)Param.getMSF()).createInstance
                ("com.sun.star.sheet.TableAutoFormats");

            XEnumerationAccess ea = (XEnumerationAccess)
                        UnoRuntime.queryInterface(XEnumerationAccess.class,oObj);

            oObj = ea.createEnumeration();

            log.println("ImplementationName: "+util.utils.getImplName(oObj));
            // creating test environment
            TestEnvironment tEnv = new TestEnvironment( oObj );

            tEnv.addObjRelation("ENUM",ea);

            return tEnv;
        } catch (com.sun.star.uno.Exception e) {
            log.println ("Exception occured while creating test Object.");
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        }
    }

}    // finish class ScAutoFormatsObj

