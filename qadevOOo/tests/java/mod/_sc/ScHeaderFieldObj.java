/*************************************************************************
 *
 *  $RCSfile: ScHeaderFieldObj.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change:$Date: 2003-01-27 18:16:29 $
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
import com.sun.star.beans.XPropertySetInfo;
import com.sun.star.container.XNameAccess;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sheet.XHeaderFooterContent;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.style.XStyle;
import com.sun.star.style.XStyleFamiliesSupplier;
import com.sun.star.text.XText;
import com.sun.star.text.XTextContent;
import com.sun.star.text.XTextCursor;
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
* <code>com.sun.star.text.TextField</code>. <p>
* Object implements the following interfaces :
* <ul>
*  <li> <code>com::sun::star::lang::XComponent</code></li>
*  <li> <code>com::sun::star::beans::XPropertySet</code></li>
*  <li> <code>com::sun::star::text::XTextField</code></li>
*  <li> <code>com::sun::star::text::XTextContent</code></li>
*  <li> <code>com::sun::star::text::TextContent</code></li>
* </ul>
* @see com.sun.star.text.TextField
* @see com.sun.star.lang.XComponent
* @see com.sun.star.beans.XPropertySet
* @see com.sun.star.text.XTextField
* @see com.sun.star.text.XTextContent
* @see com.sun.star.text.TextContent
* @see ifc.lang._XComponent
* @see ifc.beans._XPropertySet
* @see ifc.text._XTextField
* @see ifc.text._XTextContent
* @see ifc.text._TextContent
*/
public class ScHeaderFieldObj extends TestCase {
    XSpreadsheetDocument xSpreadsheetDoc;

    /**
    * Creates Spreadsheet document.
    */
    protected void initialize( TestParameters tParam, PrintWriter log ) {
        SOfficeFactory SOF = SOfficeFactory.getFactory( tParam.getMSF() );

        try {
            log.println( "creating a Spreadsheet document" );
            xSpreadsheetDoc = SOF.createCalcDoc(null);
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
            UnoRuntime.queryInterface (XComponent.class, xSpreadsheetDoc);
        oComp.dispose();
    }

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Retrieves the collection of style families available in the document
    * using the interface <code>XStyleFamiliesSupplier</code>.
    * Obtains default style from the style family <code>'PageStyles'</code>.
    * Retrieves the interface <code>XHeaderFooterContent</code> from the style
    * using the property <code>'RightPageHeaderContent'</code>. Creates the
    * instance of the service <code>com.sun.star.text.TextField.Time</code> and
    * the instance of the service <code>com.sun.star.text.TextField.Date</code>
    * Obtains the text (the interface <code>XText</code>) which is printed in
    * the left part of the header or footer and inserts in it's content
    * the second created instance.
    * Object relations created :
    * <ul>
    *  <li> <code>'CONTENT'</code> for
    *      {@link ifc.text._XTextContent}(the interface <code>XTextContent</code>
    *      that was queried from the second created instance) </li>
    *  <li> <code>'TEXT'</code> for
    *      {@link ifc.text._XTextContent}(the the text which is printed in the
    *       right part of the header or footer) </li>
    * </ul>
    * @see com.sun.star.style.XStyleFamiliesSupplier
    * @see com.sun.star.sheet.XHeaderFooterContent
    * @see com.sun.star.text.XText
    * @see com.sun.star.text.XTextContent
    */
    public TestEnvironment createTestEnvironment(
        TestParameters tParam, PrintWriter log) throws StatusException {

        XInterface oObj = null;
        XPropertySet PropSet;
        XPropertySetInfo PropSetInfo;
        XNameAccess PageStyles = null;
        XStyle StdStyle = null;
        XTextContent oContent = null;
        XInterface aField = null;

        XStyleFamiliesSupplier StyleFam = (XStyleFamiliesSupplier)
            UnoRuntime.queryInterface(
                XStyleFamiliesSupplier.class,
                xSpreadsheetDoc );

        XNameAccess StyleFamNames = StyleFam.getStyleFamilies();
        try{
            PageStyles = (XNameAccess)StyleFamNames.getByName("PageStyles");
            StdStyle = (XStyle)PageStyles.getByName("Default");
        } catch(com.sun.star.lang.WrappedTargetException e){
            e.printStackTrace(log);
            throw new StatusException("Couldn't get by name", e);
        } catch(com.sun.star.container.NoSuchElementException e){
            e.printStackTrace(log);
            throw new StatusException("Couldn't get by name", e);
        }

        //get the property-set
        PropSet = (XPropertySet)
            UnoRuntime.queryInterface(XPropertySet.class, StdStyle);

        XHeaderFooterContent RPHC = null;
        // creation of testobject here
        // first we write what we are intend to do to log file
        log.println( "creating a test environment" );
        try {
            RPHC = (XHeaderFooterContent)
                PropSet.getPropertyValue("RightPageHeaderContent");
        } catch (com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't get HeaderContent", e);
        } catch (com.sun.star.beans.UnknownPropertyException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't get HeaderContent", e);
        }

        XText left = RPHC.getLeftText();

        XMultiServiceFactory oDocMSF = (XMultiServiceFactory)
            UnoRuntime.queryInterface(
                XMultiServiceFactory.class,
                xSpreadsheetDoc );

        XTextContent the_Field = null;
        try {
            oObj = (XInterface)
                oDocMSF.createInstance( "com.sun.star.text.TextField.Time" );

            the_Field = (XTextContent)
                UnoRuntime.queryInterface(XTextContent.class,oObj);

            aField = (XInterface)
                oDocMSF.createInstance("com.sun.star.text.TextField.Date");
        } catch(com.sun.star.uno.Exception e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create instance", e);
        }

        oContent = (XTextContent)
            UnoRuntime.queryInterface(XTextContent.class, aField);

        XTextCursor the_Cursor = left.createTextCursor();

        try {
            left.insertTextContent(the_Cursor,the_Field, false);
            PropSet.setPropertyValue("RightPageHeaderContent", RPHC);
        } catch(com.sun.star.lang.IllegalArgumentException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create a test environment", e);
        } catch(com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create a test environment", e);
        } catch(com.sun.star.beans.PropertyVetoException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create a test environment", e);
        } catch(com.sun.star.beans.UnknownPropertyException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create a test environment", e);
        }

        TestEnvironment tEnv = new TestEnvironment(oObj);

        tEnv.addObjRelation("CONTENT",oContent);
        tEnv.addObjRelation("TEXT", RPHC.getRightText());

        return tEnv;

    } // finish method getTestEnvironment

}    // finish class ScHeaderFieldObj

