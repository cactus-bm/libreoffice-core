/*************************************************************************
 *
 *  $RCSfile: ODatabaseForm.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change:$Date: 2003-05-27 12:41:15 $
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

package mod._forms;

import com.sun.star.awt.XControl;
import com.sun.star.awt.XControlModel;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XIndexAccess;
import com.sun.star.container.XNameContainer;
import com.sun.star.container.XNamed;
import com.sun.star.drawing.XControlShape;
import com.sun.star.drawing.XShape;
import com.sun.star.drawing.XShapes;
import com.sun.star.form.XForm;
import com.sun.star.form.XLoadable;
import com.sun.star.io.XDataInputStream;
import com.sun.star.io.XTextInputStream;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sdb.RowChangeEvent;
import com.sun.star.sdbc.SQLException;
import com.sun.star.sdbc.XConnection;
import com.sun.star.sdbc.XResultSet;
import com.sun.star.sdbc.XResultSetUpdate;
import com.sun.star.sdbc.XRow;
import com.sun.star.sdbc.XRowSet;
import com.sun.star.sdbc.XRowUpdate;
import com.sun.star.sdbcx.XRowLocate;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.view.XControlAccess;
import java.io.PrintWriter;
import java.util.Vector;
import lib.Status;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.DBTools;
import util.DrawTools;
import util.FormTools;
import util.WriterTools;
import util.utils;

import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;

/**
 * Test for object which is represented by service
 * <code>com.sun.star.form.component.DatabaseForm</code>. <p>
 *
 * Object implements the following interfaces :
 * <ul>
 *  <li> <code>com::sun::star::script::XEventAttacherManager</code></li>
 *  <li> <code>com::sun::star::container::XElementAccess</code></li>
 *  <li> <code>com::sun::star::sdbcx::ResultSet</code></li>
 *  <li> <code>com::sun::star::container::XChild</code></li>
 *  <li> <code>com::sun::star::sdbc::XResultSetUpdate</code></li>
 *  <li> <code>com::sun::star::sdb::XResultSetAccess</code></li>
 *  <li> <code>com::sun::star::form::FormComponent</code></li>
 *  <li> <code>com::sun::star::form::component::DataForm</code></li>
 *  <li> <code>com::sun::star::sdbc::XResultSetMetaDataSupplier</code></li>
 *  <li> <code>com::sun::star::container::XEnumerationAccess</code></li>
 *  <li> <code>com::sun::star::sdbcx::XDeleteRows</code></li>
 *  <li> <code>com::sun::star::sdb::RowSet</code></li>
 *  <li> <code>com::sun::star::lang::XComponent</code></li>
 *  <li> <code>com::sun::star::sdbc::XRowSet</code></li>
 *  <li> <code>com::sun::star::sdbc::XRowUpdate</code></li>
 *  <li> <code>com::sun::star::form::XLoadable</code></li>
 *  <li> <code>com::sun::star::container::XNamed</code></li>
 *  <li> <code>com::sun::star::container::XIndexReplace</code></li>
 *  <li> <code>com::sun::star::io::XPersistObject</code></li>
 *  <li> <code>com::sun::star::container::XNameReplace</code></li>
 *  <li> <code>com::sun::star::container::XIndexContainer</code></li>
 *  <li> <code>com::sun::star::container::XNameAccess</code></li>
 *  <li> <code>com::sun::star::sdbc::XParameters</code></li>
 *  <li> <code>com::sun::star::util::XCancellable</code></li>
 *  <li> <code>com::sun::star::form::XReset</code></li>
 *  <li> <code>com::sun::star::sdbc::XCloseable</code></li>
 *  <li> <code>com::sun::star::sdbcx::XColumnsSupplier</code></li>
 *  <li> <code>com::sun::star::sdb::XRowSetApproveBroadcaster</code></li>
 *  <li> <code>com::sun::star::sdbc::ResultSet</code></li>
 *  <li> <code>com::sun::star::sdbc::XResultSet</code></li>
 *  <li> <code>com::sun::star::sdbc::XRow</code></li>
 *  <li> <code>com::sun::star::sdbc::XColumnLocate</code></li>
 *  <li> <code>com::sun::star::awt::XTabControllerModel</code></li>
 *  <li> <code>com::sun::star::container::XIndexAccess</code></li>
 *  <li> <code>com::sun::star::form::XSubmit</code></li>
 *  <li> <code>com::sun::star::form::component::HTMLForm</code></li>
 *  <li> <code>com::sun::star::sdbcx::XRowLocate</code></li>
 *  <li> <code>com::sun::star::sdbc::XWarningsSupplier</code></li>
 *  <li> <code>com::sun::star::container::XNameContainer</code></li>
 *  <li> <code>com::sun::star::beans::XPropertySet</code></li>
 *  <li> <code>com::sun::star::sdbc::RowSet</code></li>
 * </ul> <p>
 *
 * This object test <b> is NOT </b> designed to be run in several
 * threads concurently.
 * The following parameters in ini-file used by this test:
 * <ul>
 *   <li><code>test.db.url</code> - URL to MySQL database.
 *   For example: <code>mysql://mercury:3306/api_current</code></li>
 *   <li><code>test.db.user</code> - user for MySQL database</li>
 *   <li><code>test.db.password</code> - password for MySQL database</li>
 * </ul><p>
 *
 * @see com.sun.star.script.XEventAttacherManager
 * @see com.sun.star.container.XElementAccess
 * @see com.sun.star.sdbcx.ResultSet
 * @see com.sun.star.container.XChild
 * @see com.sun.star.sdbc.XResultSetUpdate
 * @see com.sun.star.sdb.XResultSetAccess
 * @see com.sun.star.form.FormComponent
 * @see com.sun.star.form.component.DataForm
 * @see com.sun.star.sdbc.XResultSetMetaDataSupplier
 * @see com.sun.star.container.XEnumerationAccess
 * @see com.sun.star.sdbcx.XDeleteRows
 * @see com.sun.star.sdb.RowSet
 * @see com.sun.star.lang.XComponent
 * @see com.sun.star.sdbc.XRowSet
 * @see com.sun.star.sdbc.XRowUpdate
 * @see com.sun.star.form.XLoadable
 * @see com.sun.star.container.XNamed
 * @see com.sun.star.container.XIndexReplace
 * @see com.sun.star.io.XPersistObject
 * @see com.sun.star.container.XNameReplace
 * @see com.sun.star.container.XIndexContainer
 * @see com.sun.star.container.XNameAccess
 * @see com.sun.star.sdbc.XParameters
 * @see com.sun.star.util.XCancellable
 * @see com.sun.star.form.XReset
 * @see com.sun.star.sdbc.XCloseable
 * @see com.sun.star.sdbcx.XColumnsSupplier
 * @see com.sun.star.sdb.XRowSetApproveBroadcaster
 * @see com.sun.star.sdbc.ResultSet
 * @see com.sun.star.sdbc.XResultSet
 * @see com.sun.star.sdbc.XRow
 * @see com.sun.star.sdbc.XColumnLocate
 * @see com.sun.star.awt.XTabControllerModel
 * @see com.sun.star.container.XIndexAccess
 * @see com.sun.star.form.XSubmit
 * @see com.sun.star.form.component.HTMLForm
 * @see com.sun.star.sdbcx.XRowLocate
 * @see com.sun.star.sdbc.XWarningsSupplier
 * @see com.sun.star.container.XNameContainer
 * @see com.sun.star.beans.XPropertySet
 * @see com.sun.star.sdbc.RowSet
 * @see ifc.script._XEventAttacherManager
 * @see ifc.container._XElementAccess
 * @see ifc.sdbcx._ResultSet
 * @see ifc.container._XChild
 * @see ifc.sdbc._XResultSetUpdate
 * @see ifc.sdb._XResultSetAccess
 * @see ifc.form._FormComponent
 * @see ifc.form.component._DataForm
 * @see ifc.sdbc._XResultSetMetaDataSupplier
 * @see ifc.container._XEnumerationAccess
 * @see ifc.sdbcx._XDeleteRows
 * @see ifc.sdb._RowSet
 * @see ifc.lang._XComponent
 * @see ifc.sdbc._XRowSet
 * @see ifc.sdbc._XRowUpdate
 * @see ifc.form._XLoadable
 * @see ifc.container._XNamed
 * @see ifc.container._XIndexReplace
 * @see ifc.io._XPersistObject
 * @see ifc.container._XNameReplace
 * @see ifc.container._XIndexContainer
 * @see ifc.container._XNameAccess
 * @see ifc.sdbc._XParameters
 * @see ifc.util._XCancellable
 * @see ifc.form._XReset
 * @see ifc.sdbc._XCloseable
 * @see ifc.sdbcx._XColumnsSupplier
 * @see ifc.sdb._XRowSetApproveBroadcaster
 * @see ifc.sdbc._ResultSet
 * @see ifc.sdbc._XResultSet
 * @see ifc.sdbc._XRow
 * @see ifc.sdbc._XColumnLocate
 * @see ifc.awt._XTabControllerModel
 * @see ifc.container._XIndexAccess
 * @see ifc.form._XSubmit
 * @see ifc.form.component._HTMLForm
 * @see ifc.sdbcx._XRowLocate
 * @see ifc.sdbc._XWarningsSupplier
 * @see ifc.container._XNameContainer
 * @see ifc.beans._XPropertySet
 * @see ifc.sdbc._RowSet
 */
public class ODatabaseForm extends TestCase {

    protected XTextDocument xTextDoc = null;

    protected final static String dbSourceName = "ODatabaseFormDataSource" ;

    private static int uniqueSuffix = 0 ;
    private DBTools dbTools     = null ;
    private static String origDB = null ;
    private static String tmpDir = null ;
    String tableName = null;
    DBTools.DataSourceInfo srcInf = null;
    boolean isMySQLDB = false;
    protected XConnection conn = null;


    protected void initialize( TestParameters tParam, PrintWriter log ) {
        //log.println( "creating a draw document" );
        //xTextDoc = WriterTools.createTextDoc((XMultiServiceFactory)tParam.getMSF());

        tmpDir = utils.getOfficeTemp((XMultiServiceFactory)tParam.getMSF());

        origDB = util.utils.getFullTestDocName("TestDB/testDB.dbf");

        dbTools = new DBTools((XMultiServiceFactory)tParam.getMSF()) ;

        // creating DataSource and registering it in DatabaseContext

        String dbURL = (String) tParam.get("test.db.url");
        String dbUser = (String) tParam.get("test.db.user");
        String dbPassword = (String) tParam.get("test.db.password");

        log.println("Creating and registering DataSource ...");
        srcInf = dbTools.newDataSourceInfo();
        if (dbURL != null && dbUser != null && dbPassword != null) {
            isMySQLDB = true;
            log.println("dbURL = " + dbURL);
            log.println("dbUSER = " + dbUser);
            log.println("dbPASSWORD = " + dbPassword);
            //DataSource for mysql db
            try {
                tableName = "soffice_test_table";
                srcInf.URL = "jdbc:" + dbURL;
                srcInf.IsPasswordRequired = new Boolean(true);
                srcInf.Password = dbPassword;
                srcInf.User = dbUser;
                PropertyValue[] propInfo = new PropertyValue[1];
                propInfo[0] = new PropertyValue();
                propInfo[0].Name = "JavaDriverClass";
                propInfo[0].Value = "org.gjt.mm.mysql.Driver";
                srcInf.Info = propInfo;
                Object dbSrc = srcInf.getDataSourceService() ;
                dbTools.reRegisterDB(dbSourceName, dbSrc);
            } catch (com.sun.star.uno.Exception e) {
                log.println("Error while object test initialization :") ;
                e.printStackTrace(log) ;
                throw new StatusException("Error while object test" +
                    " initialization", e);
            }
        } else {
            //DataSource for sdbc db
            try {
                srcInf.URL = "sdbc:dbase:" + DBTools.dirToUrl(tmpDir) ;
                Object dbSrc = srcInf.getDataSourceService() ;
                dbTools.reRegisterDB(dbSourceName, dbSrc) ;
            } catch (com.sun.star.uno.Exception e) {
                log.println("Error while object test initialization :") ;
                e.printStackTrace(log) ;
                throw new StatusException
                    ("Error while object test initialization",e) ;
            }

            String oldF = null ;
            String newF = null ;
            do {
                tableName = "ODatabaseForm_tmp" + uniqueSuffix ;
                oldF = utils.getFullURL(origDB);
                newF = utils.getOfficeTemp((XMultiServiceFactory)tParam.getMSF()) +
                    tableName + ".dbf";
            } while (!utils.overwriteFile((XMultiServiceFactory)tParam.getMSF(), oldF, newF)
                     && uniqueSuffix++ < 50);
            }
    }

    /**
     *  *    creating a Testenvironment for the interfaces to be tested
     */
    protected synchronized TestEnvironment createTestEnvironment(TestParameters Param, PrintWriter log) {

        if (xTextDoc != null) {
            xTextDoc.dispose();
            log.println("Exiting document disposed");
        }

        log.println( "creating a text document" );
        xTextDoc = WriterTools.createTextDoc((XMultiServiceFactory)Param.getMSF());

        //initialize test table
        if (isMySQLDB) {
            try {
                dbTools.initTestTableUsingJDBC(tableName, srcInf);
            } catch(java.sql.SQLException e) {
                e.printStackTrace(log);
                throw new StatusException(Status.failed("Couldn't " +
                    " init test table. SQLException..."));
            } catch(java.lang.ClassNotFoundException e) {
                throw new StatusException(Status.failed("Couldn't " +
                    "register mysql driver"));
            }
        }

        XInterface oObj = null;
        XShapes oShapes = null;
        XInterface oInstance = null;
        XConnection connection = null ;

        // creation of testobject here
        // first we write what we are intend to do to log file
        log.println( "creating a test environment" );

        XNameContainer forms = FormTools.getForms
            (WriterTools.getDrawPage(xTextDoc)) ;
        try {
            String[] formNames = forms.getElementNames();
            for (int i = 0; i < formNames.length; i++) {
                log.println("Removing form '" + formNames[i] + "' ...");
                forms.removeByName(formNames[i]) ;
            }
        } catch (com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log);
        } catch (com.sun.star.container.NoSuchElementException e) {
            e.printStackTrace(log);
        }

        String[] formNames = forms.getElementNames();
        FormTools.insertForm(xTextDoc, forms, "MyForm");
        formNames = forms.getElementNames();

        XLoadable formLoader = null ;
        try {
            formLoader = FormTools.bindForm(xTextDoc, "MyForm",
            dbSourceName, tableName);
        } catch (com.sun.star.uno.Exception e) {
            log.println("Cann't bind the form to source '" +
                dbSourceName + "', table '" + tableName + "' :");
            e.printStackTrace(log);
            throw new StatusException("Cann't bind a form", e);
        }

        // DEBUG
        log.println("Forms before adding controls : ") ;
        formNames = forms.getElementNames() ;
        for (int i = 0; i < formNames.length; i++) {
            log.println("    '" + formNames[i] + "'");
        }

        XControlShape shape1 = null ;
        XControlShape shape2 = null ;
        try {
            XNameContainer formElements = (XNameContainer)
                UnoRuntime.queryInterface
                (XNameContainer.class, forms.getByName("MyForm")) ;

            log.println("Elements in the 'MyForm' :") ;
            XIndexAccess formElements1 = (XIndexAccess)
                UnoRuntime.queryInterface
                (XIndexAccess.class, forms.getByName("MyForm")) ;
            for (int i = 0; i < formElements1.getCount(); i++) {
                XNamed elemName = (XNamed) UnoRuntime.queryInterface
                    (XNamed.class, formElements1.getByIndex(i)) ;
                log.println("   '" + elemName.getName() + "'");
            }
            // END DEBUG

            //put something on the drawpage
            log.println( "inserting some ControlShapes" );
            oShapes = DrawTools.getShapes(WriterTools.getDrawPage(xTextDoc));
            shape1 = FormTools.createControlShape(
                xTextDoc,3000,4500,15000,1000,"CommandButton");
            shape2 = FormTools.createControlShape(
                xTextDoc,5000,3500,7500,5000,"TextField");
            XControlShape shape3 = FormTools.createControlShape(
                xTextDoc,2000,1500,1000,1000,"CheckBox");
            oShapes.add((XShape) shape1);
            oShapes.add((XShape) shape2);
            oShapes.add(shape3) ;
        } catch (Exception e) {
            e.printStackTrace(log);
        }

        log.println("Forms after adding controls : ") ;
        formNames = forms.getElementNames() ;
        for (int i = 0; i < formNames.length; i++) {
            log.println("    '" + formNames[i] + "'");
        }

        try {
            log.println("Elements in the 'MyForm' :") ;
            XIndexAccess formElements1 = (XIndexAccess)
                UnoRuntime.queryInterface
                (XIndexAccess.class, forms.getByName("MyForm")) ;
            for (int i = 0; i < formElements1.getCount(); i++) {
                XNamed elemName = (XNamed) UnoRuntime.queryInterface
                    (XNamed.class, formElements1.getByIndex(i)) ;
                log.println("   '" + elemName.getName() + "'");
            }
        } catch (Exception e) {
            e.printStackTrace(log);
        }

        formLoader.load();

        try {
            oObj = (XForm) AnyConverter.toObject(new Type(XForm.class),
                (FormTools.getForms(
                        WriterTools.getDrawPage(xTextDoc))).getByName("MyForm"));

            XPropertySet xSetProp = (XPropertySet) UnoRuntime.queryInterface
                (XPropertySet.class, oObj) ;
            connection = (XConnection) AnyConverter.toObject(new Type(XConnection.class),
                xSetProp.getPropertyValue("ActiveConnection")) ;
        } catch ( com.sun.star.uno.Exception e ) {
            log.println("Couldn't get Form");
            e.printStackTrace(log);
        }

        // get a control
        oInstance = FormTools.createControl(xTextDoc, "TextField");

        log.println( "creating a new environment for drawpage object" );
        TestEnvironment tEnv = new TestEnvironment( oObj );

        // adding relation for closing connection while environment disposing.
        this.conn = connection;

        // adding relation for XSubmit
        XControlModel the_Model = shape2.getControl() ;
        XControlAccess the_access = (XControlAccess) UnoRuntime.queryInterface(
            XControlAccess.class,xTextDoc.getCurrentController());
        XControl cntrl = null ;

        //now get the OEditControl
        try {
            cntrl = the_access.getControl(the_Model);
        } catch (com.sun.star.container.NoSuchElementException e) {
            log.println("Couldn't get OEditControl");
            e.printStackTrace(log);
            throw new StatusException("Couldn't get OEditControl", e );
        }

        XResultSet the_set = (XResultSet) UnoRuntime.queryInterface
            (XResultSet.class,oObj);

        try {
            the_set.first();
        } catch (SQLException e) {
            log.println("Cann't move cursor to the first row.");
            throw new StatusException
                ("Cann't move cursor to the first row.", e) ;
        }

        tEnv.addObjRelation("Model1",shape1.getControl());
        tEnv.addObjRelation("Model2",shape2.getControl());

        // adding an object for XNameReplace testing
        log.println( "adding oInstace as obj relation to environment" );
        tEnv.addObjRelation("INSTANCE", oInstance);

        // INDEX : _XNameContainer
        log.println( "adding INDEX as obj relation to environment" );
        tEnv.addObjRelation("INDEX", "0");

        // INDEX : _XNameReplace
        log.println( "adding NameReplaceIndex as obj relation to environment" );
        tEnv.addObjRelation("XNameReplaceINDEX", "2");

        // INSTANCEn : _XNameContainer; _XNameReplace
        log.println( "adding INSTANCEn as obj relation to environment" );

        //XComponent xComp = (XComponent)
        //    UnoRuntime.queryInterface(XComponent.class, xDrawDoc);
        String tc = (String)Param.get("THRCNT");
        int THRCNT = 1;
        if (tc != null) {
            THRCNT = Integer.parseInt(tc);
        }
        for (int n = 1; n < 2*(THRCNT+1) ;n++ ) {
            log.println( "adding INSTANCE" + n
                +" as obj relation to environment" );
            tEnv.addObjRelation("INSTANCE" + n,
                                FormTools.createControl(xTextDoc,"CheckBox"));
        }

        // adding relation for XNameContainer
        tEnv.addObjRelation("XNameContainer.AllowDuplicateNames", new Object());

        // adding relation for XPersistObject
        tEnv.addObjRelation("OBJNAME", "stardiv.one.form.component.Form") ;

        if (the_set != null) log.println("The Form has a not empty ResultSet");

            // Adding obj relation for XRowSetApproveBroadcaster test
            final XResultSet xResSet = (XResultSet) UnoRuntime.queryInterface
                (XResultSet.class, oObj) ;
            final XResultSetUpdate xResSetUpdate = (XResultSetUpdate)
                UnoRuntime.queryInterface(XResultSetUpdate.class, oObj) ;
            final XRowSet xRowSet = (XRowSet) UnoRuntime.queryInterface
                (XRowSet.class, oObj) ;
            final PrintWriter logF = log ;
            tEnv.addObjRelation("XRowSetApproveBroadcaster.ApproveChecker",
                new ifc.sdb._XRowSetApproveBroadcaster.RowSetApproveChecker() {
                    public void moveCursor() {
                        try {
                            xResSet.beforeFirst() ;
                            xResSet.afterLast() ;
                        } catch (com.sun.star.sdbc.SQLException e) {
                            logF.println("### _XRowSetApproveBroadcaster."
                                +"RowSetApproveChecker.moveCursor() :") ;
                            e.printStackTrace(logF) ;
                        }
                    }
                    public RowChangeEvent changeRow() {
                        try {
                            xResSet.first() ;
                            XRowUpdate row = (XRowUpdate)
                                UnoRuntime.queryInterface
                                (XRowUpdate.class, xResSet);
                            row.updateString(1, "1");
                            xResSetUpdate.updateRow() ;
                        } catch (com.sun.star.sdbc.SQLException e) {
                            logF.println("### _XRowSetApproveBroadcaster."
                                +"RowSetApproveChecker.changeRow() :") ;
                            e.printStackTrace(logF) ;
                        }
                        RowChangeEvent ev = new RowChangeEvent() ;
                        ev.Action = com.sun.star.sdb.RowChangeAction.UPDATE ;
                        ev.Rows = 1 ;

                        return ev ;
                    }
                    public void changeRowSet() {
                        try {
                            xRowSet.execute() ;
                        } catch (com.sun.star.sdbc.SQLException e) {
                            logF.println("### _XRowSetApproveBroadcaster."
                                +"RowSetApproveChecker.changeRowSet() :") ;
                            e.printStackTrace(logF) ;
                        }
                    }
                }) ;

        // Adding relation for XColumnLocate test
        tEnv.addObjRelation("XColumnLocate.ColumnName", DBTools.TST_STRING_F) ;

        // Adding relation for XParameters ifc test
        Vector params = new Vector() ;

        /*****  statement parameter types and their initial
                values must be added here as relation.
        params.add(new String("SAU99")) ;
        params.add(new Boolean(false)) ;
        params.add(new Byte((byte) 123)) ;
        params.add(new Short((short) 234)) ;
        params.add(new Integer(12345)) ;
        params.add(new Long(23456)) ;
        params.add(new Float(1.234)) ;
        params.add(new Double(2.345)) ;
        params.add(new byte[] {1, 2, 3}) ;
        params.add(new Date(26, 1, 2001)) ;
        params.add(new Time(1, 25, 14, 12)) ;
        params.add(new DateTime(1, 25, 14, 12, 26, 1, 2001)) ;
        */
        tEnv.addObjRelation("XParameters.ParamValues", params) ;

        // Adding relation for XDeleteRows ifc test
        final XRowLocate rowLoc = (XRowLocate)
            UnoRuntime.queryInterface(XRowLocate.class, oObj);
        final XResultSet bMarkResSet = (XResultSet)
            UnoRuntime.queryInterface(XResultSet.class, oObj);

        // Adding relation for XResultSetUpdate
        final XRowUpdate xRowUpdate = (XRowUpdate)
            UnoRuntime.queryInterface(XRowUpdate.class, oObj) ;
        final XRow xRow = (XRow) UnoRuntime.queryInterface
            (XRow.class, oObj) ;

        tEnv.addObjRelation("XResultSetUpdate.UpdateTester",
            new ifc.sdbc._XResultSetUpdate.UpdateTester() {
                String lastUpdate = null ;

                public int rowCount() throws SQLException {
                    int prevPos = xResSet.getRow() ;
                    xResSet.last() ;
                    int count = xResSet.getRow() ;
                    xResSet.absolute(prevPos) ;

                    return count ;
                }

                public void update() throws SQLException {
                    lastUpdate = xRow.getString(1) ;
                    lastUpdate += "_" ;
                    xRowUpdate.updateString(1, lastUpdate) ;
                }

                public boolean wasUpdated() throws SQLException {
                    String getStr = xRow.getString(1) ;
                    return lastUpdate.equals(getStr) ;
                }

                public int currentRow() throws SQLException {
                    return xResSet.getRow() ;
                }
            }) ;

        // Adding relations for XRow as a Vector with all data
        // of current row of RowSet.
        try {
            XMultiServiceFactory xMSF = (XMultiServiceFactory)Param.getMSF() ;
            Object oBinStream = xMSF.
                createInstance("com.sun.star.io.DataInputStream") ;
            XDataInputStream xBinStream = (XDataInputStream)
                UnoRuntime.queryInterface(XDataInputStream.class,
                    oBinStream) ;
            Object oCharStream = xMSF.
                createInstance("com.sun.star.io.TextInputStream");
            XTextInputStream xCharStream = (XTextInputStream)
                UnoRuntime.queryInterface(XTextInputStream.class,
                    oCharStream);
            Object obj = xMSF.createInstance("com.sun.star.io.Pipe");
        } catch (com.sun.star.uno.Exception e) {
            log.println("Object relation cannot be created");
            e.printStackTrace(log);
            throw new StatusException(
                "Object relation cannot be created", e);
        }

        Vector rowData = new Vector();

        for (int i = 0; i < DBTools.TST_TABLE_VALUES[0].length; i++) {
            rowData.add(DBTools.TST_TABLE_VALUES[0][i]);
        }

        tEnv.addObjRelation("CurrentRowData", rowData);

        // Adding relation for XRowUpdate
        XRow row = (XRow) UnoRuntime.queryInterface(XRow.class, oObj);
        tEnv.addObjRelation("XRowUpdate.XRow", row);

        return tEnv;
    } // finish method getTestEnvironment

    /**
    * Closes connection of <code>RowSet</code> instance created.
    */
    protected void cleanup( TestParameters Param, PrintWriter log) {
        try {
            conn.close();
        } catch (com.sun.star.uno.Exception e) {
            log.println("Can't close the connection");
            e.printStackTrace(log);
        } catch (com.sun.star.lang.DisposedException e) {
            log.println("Connection was already closed. It's OK.");
        }
        log.println( "    disposing xTextDoc " );
        xTextDoc.dispose();

        try {
            dbTools.revokeDB(dbSourceName) ;
        } catch (com.sun.star.uno.Exception e) {
            log.println("Error while object test cleaning up :") ;
            e.printStackTrace(log) ;
            throw new StatusException("Error while object test cleaning up",e) ;
        }
    }

}    // finish class ODatabaseForm

