/*************************************************************************
 *
 *  $RCSfile: OHiddenModel.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change:$Date: 2003-02-10 10:48:19 $
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

import com.sun.star.container.XNameContainer;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.lang.XComponent;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import java.io.PrintWriter;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.DrawTools;
import util.FormTools;
import util.SOfficeFactory;

import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;

/**
* Test for object which is represented by service
* <code>com.sun.star.form.component.HiddenControl</code>. <p>
* Object implements the following interfaces :
* <ul>
*  <li> <code>com::sun::star::io::XPersistObject</code></li>
*  <li> <code>com::sun::star::container::XChild</code></li>
*  <li> <code>com::sun::star::form::FormControlModel</code></li>
*  <li> <code>com::sun::star::form::component::HiddenControl</code></li>
*  <li> <code>com::sun::star::form::FormComponent</code></li>
*  <li> <code>com::sun::star::beans::XPropertySet</code></li>
*  <li> <code>com::sun::star::beans::XFastPropertySet</code></li>
*  <li> <code>com::sun::star::beans::XPropertyState</code></li>
*  <li> <code>com::sun::star::container::XNamed</code></li>
*  <li> <code>com::sun::star::beans::XMultiPropertySet</code></li>
*  <li> <code>com::sun::star::lang::XComponent</code></li>
* </ul> <p>
* This object test <b> is NOT </b> designed to be run in several
* threads concurently.
* @see com.sun.star.io.XPersistObject
* @see com.sun.star.container.XChild
* @see com.sun.star.form.FormControlModel
* @see com.sun.star.form.component.HiddenControl
* @see com.sun.star.form.FormComponent
* @see com.sun.star.beans.XPropertySet
* @see com.sun.star.beans.XFastPropertySet
* @see com.sun.star.beans.XPropertyState
* @see com.sun.star.container.XNamed
* @see com.sun.star.beans.XMultiPropertySet
* @see com.sun.star.lang.XComponent
* @see ifc.io._XPersistObject
* @see ifc.container._XChild
* @see ifc.form._FormControlModel
* @see ifc.form.component._HiddenControl
* @see ifc.form._FormComponent
* @see ifc.beans._XPropertySet
* @see ifc.beans._XFastPropertySet
* @see ifc.beans._XPropertyState
* @see ifc.container._XNamed
* @see ifc.beans._XMultiPropertySet
* @see ifc.lang._XComponent
*/
public class OHiddenModel extends TestCase {

    XComponent xDrawDoc;

    /**
    * Creates Drawing document.
    */
    protected void initialize( TestParameters tParam, PrintWriter log ) {
        SOfficeFactory SOF = SOfficeFactory.getFactory( tParam.getMSF() );

        try {
            log.println( "creating a draw document" );
            xDrawDoc = SOF.createDrawDoc(null);;
        } catch ( com.sun.star.uno.Exception e ) {
            // Some exception occures.FAILED
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }

    }

    /**
    * Disposes drawing document.
    */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xDrawDoc " );
        xDrawDoc.dispose();
    }

    /**
    * Creates hidden component, then adds Form into draw page,
    * and inserts the component into Form components' collection.
    *     Object relations created :
    * <ul>
    *  <li> <code>'OBJNAME'</code> for
    *      {@link ifc.io._XPersistObject} : name of service which is
    *    represented by this object. </li>
    * </ul>
    */
    protected synchronized TestEnvironment createTestEnvironment(TestParameters Param, PrintWriter log) {


        XInterface oObj = null;

        // creation of testobject here
        // first we write what we are intend to do to log file
        log.println( "creating a test environment" );

        // get a soffice factory object
        SOfficeFactory SOF = SOfficeFactory.getFactory( Param.getMSF());
        String objName = "HiddenControl";
        XInterface ctrl = SOF.createControl(xDrawDoc, objName) ;

        try {
            XDrawPage oDP = DrawTools.getDrawPage(xDrawDoc,0);

            XNameContainer nc = FormTools.getForms(oDP);
            FormTools.insertForm(xDrawDoc, nc, "OHiddenModelForm");

            Object frm = nc.getByName("OHiddenModelForm") ;

            XNameContainer frmNC = (XNameContainer) UnoRuntime.queryInterface
                (XNameContainer.class, frm) ;

            frmNC.insertByName("OHiddenModel", ctrl) ;
            oObj = (XInterface) AnyConverter.toObject(
                new Type(XInterface.class),frmNC.getByName("OHiddenModel"));
        } catch (com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log) ;
            throw new StatusException("Can't create and add control", e) ;
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            e.printStackTrace(log) ;
            throw new StatusException("Can't create and add control", e) ;
        } catch (com.sun.star.container.NoSuchElementException e) {
            e.printStackTrace(log) ;
            throw new StatusException("Can't create and add control", e) ;
        } catch (com.sun.star.container.ElementExistException e) {
            e.printStackTrace(log) ;
            throw new StatusException("Can't create and add control", e) ;
        }

        log.println( "creating a new environment for drawpage object" );
        TestEnvironment tEnv = new TestEnvironment( oObj );

        util.dbg.getSuppServices(oObj);

        log.println( "adding DrawDocument as obj relation to environment" );
        tEnv.addObjRelation("OBJNAME", "stardiv.one.form.component.Hidden");

        return tEnv;
    } // finish method getTestEnvironment

}    // finish class OHiddenModel

