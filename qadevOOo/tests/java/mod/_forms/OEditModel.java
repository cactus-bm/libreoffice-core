/*************************************************************************
 *
 *  $RCSfile: OEditModel.java,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change:$Date: 2005-02-24 17:43:02 $
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

import com.sun.star.beans.NamedValue;
import java.io.PrintWriter;

import lib.TestEnvironment;
import lib.TestParameters;
import util.DBTools;



/**
* Test for object which is represented by service
* <code>com.sun.star.form.component.TextField</code>. <p>
* Object implements the following interfaces :
* <ul>
*  <li> <code>com::sun::star::io::XPersistObject</code></li>
*  <li> <code>com::sun::star::form::XReset</code></li>
*  <li> <code>com::sun::star::form::XBoundComponent</code></li>
*  <li> <code>com::sun::star::form::FormComponent</code></li>
*  <li> <code>com::sun::star::beans::XFastPropertySet</code></li>
*  <li> <code>com::sun::star::beans::XMultiPropertySet</code></li>
*  <li> <code>com::sun::star::form::XUpdateBroadcaster</code></li>
*  <li> <code>com::sun::star::form::component::TextField</code></li>
*  <li> <code>com::sun::star::form::DataAwareControlModel</code></li>
*  <li> <code>com::sun::star::beans::XPropertyState</code></li>
*  <li> <code>com::sun::star::form::FormControlModel</code></li>
*  <li> <code>com::sun::star::container::XNamed</code></li>
*  <li> <code>com::sun::star::awt::UnoControlEditModel</code></li>
*  <li> <code>com::sun::star::form::component::DatabaseTextField</code></li>
*  <li> <code>com::sun::star::lang::XComponent</code></li>
*  <li> <code>com::sun::star::lang::XEventListener</code></li>
*  <li> <code>com::sun::star::beans::XPropertySet</code></li>
*  <li> <code>com::sun::star::form::XLoadListener</code></li>
*  <li> <code>com::sun::star::container::XChild</code></li>
* </ul>
* The following files used by this test :
* <ul>
*  <li><b> TestDB </b> (directory) : directory with test database </li>
*  <li><b> TestDB/TestDB.dbf </b> : table file. See
*    {@link util.DBTools DBTools} class for more information.</li>
* </ul> <p>
* This object test <b> is NOT </b> designed to be run in several
* threads concurently.
* @see com.sun.star.io.XPersistObject
* @see com.sun.star.form.XReset
* @see com.sun.star.form.XBoundComponent
* @see com.sun.star.form.FormComponent
* @see com.sun.star.beans.XFastPropertySet
* @see com.sun.star.beans.XMultiPropertySet
* @see com.sun.star.form.XUpdateBroadcaster
* @see com.sun.star.form.component.TextField
* @see com.sun.star.form.DataAwareControlModel
* @see com.sun.star.beans.XPropertyState
* @see com.sun.star.form.FormControlModel
* @see com.sun.star.container.XNamed
* @see com.sun.star.awt.UnoControlEditModel
* @see com.sun.star.form.component.DatabaseTextField
* @see com.sun.star.lang.XComponent
* @see com.sun.star.lang.XEventListener
* @see com.sun.star.beans.XPropertySet
* @see com.sun.star.form.XLoadListener
* @see com.sun.star.container.XChild
* @see ifc.io._XPersistObject
* @see ifc.form._XReset
* @see ifc.form._XBoundComponent
* @see ifc.form._FormComponent
* @see ifc.beans._XFastPropertySet
* @see ifc.beans._XMultiPropertySet
* @see ifc.form._XUpdateBroadcaster
* @see ifc.form.component._TextField
* @see ifc.form._DataAwareControlModel
* @see ifc.beans._XPropertyState
* @see ifc.form._FormControlModel
* @see ifc.container._XNamed
* @see ifc.awt._UnoControlEditModel
* @see ifc.form.component._DatabaseTextField
* @see ifc.lang._XComponent
* @see ifc.lang._XEventListener
* @see ifc.beans._XPropertySet
* @see ifc.form._XLoadListener
* @see ifc.container._XChild
*/
public class OEditModel extends GenericModelTest {

    /**
     * Set some member variable of the super class <CODE>GenericModelTest</CODE>:
     * <pre>
     *    <super.m_ChangePropertyName</CODE> = "Text";
     *    super.m_kindOfControl</CODE>="TextField";
     *    super.m_ObjectName</CODE> = "Edit";
     *    NamedValue myProp = new NamedValue();
     *        myProp.Name = "DataField";
     *        myProp.Value = DBTools.TST_STRING_F;
     *        super.m_propertiesToSet.add(myProp);
     *        super.m_LCShape_Type = "FixedText";
     * </pre>
     * Then <CODE>super.initialize()</CODE> was called.
     * @param tParam the test parameter
     * @param log the log writer
     */

    protected void initialize(TestParameters tParam, PrintWriter log) {
        super.m_ChangePropertyName = "Text";

        super.m_kindOfControl="TextField";

        super.m_ObjectName = "stardiv.one.form.component.Edit";

        NamedValue myProp = new NamedValue();
        myProp.Name = "DataField";
        myProp.Value = DBTools.TST_STRING_F;
        super.m_propertiesToSet.add(myProp);

        super.m_LCShape_Type = "FixedText";

        super.initialize(tParam, log);
    }

    /**
     * calls <CODE>cleanup()</CODE> from it's super class
     * @param tParam the test parameter
     * @param log the log writer
     */
    protected void cleanup(TestParameters tParam, PrintWriter log) {
        super.cleanup(tParam, log);
    }


    /**
     * calls <CODE>createTestEnvironment()</CODE> from it's super class
     * @param Param the test parameter
     * @param log the log writer
     * @return lib.TestEnvironment
     */
    protected synchronized TestEnvironment createTestEnvironment(TestParameters Param,
                                                                 PrintWriter log) {
        return super.createTestEnvironment(Param, log);
    }
} // finish class OEditModel
