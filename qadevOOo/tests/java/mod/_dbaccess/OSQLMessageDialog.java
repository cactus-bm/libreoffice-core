/*************************************************************************
 *
 *  $RCSfile: OSQLMessageDialog.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change:$Date: 2003-05-27 12:37:48 $
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

package mod._dbaccess;

import com.sun.star.uno.XInterface;
import com.sun.star.lang.XMultiServiceFactory;
import java.io.PrintWriter;
import lib.Status;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;

/**
 * Test for object which is represented by service
 * <code>com.sun.star.sdb.ErrorMessageDialog</code>. <p>
 * Object implements the following interfaces :
 * <ul>
 *  <li> <code>com::sun::star::lang::XInitialization</code></li>
 *  <li> <code>com::sun::star::sdb::ErrorMessageDialog</code></li>
 *  <li> <code>com::sun::star::ui::dialogs::XExecutableDialog</code></li>
 *  <li> <code>com::sun::star::beans::XPropertySet</code></li>
 * </ul> <p>
 * This object test <b> is NOT </b> designed to be run in several
 * threads concurently.
 *
 * @see com.sun.star.lang.XInitialization
 * @see com.sun.star.sdb.ErrorMessageDialog
 * @see com.sun.star.ui.dialogs.XExecutableDialog
 * @see com.sun.star.beans.XPropertySet
 * @see ifc.lang._XInitialization
 * @see ifc.sdb._ErrorMessageDialog
 * @see ifc.ui.dialogs._XExecutableDialog
 * @see ifc.beans._XPropertySet
 */
public class OSQLMessageDialog extends TestCase {

    /**
     * Creating a Testenvironment for the interfaces to be tested.
     *
     * Creates an instance of the service
     * <code>com.sun.star.sdb.ErrorMessageDialog</code>.
     *     Object relations created :
     * <ul>
     *  <li> <code>'ERR1', 'ERR2'</code> for
     *      {@link ifc.sdb._ErrorMessageDialog} </li>
     * </ul>
     */
    public TestEnvironment createTestEnvironment( TestParameters Param,
        PrintWriter log ) throws StatusException {
        XInterface oObj = null;
        Object oInterface = null;

        Object err1 = null;
        Object err2 = null;

        try {
            oInterface = ((XMultiServiceFactory)Param.getMSF()).createInstance(
                            "com.sun.star.sdb.ErrorMessageDialog" );

            err1 = new com.sun.star.sdbc.SQLException("err1");
            err2 = new com.sun.star.sdbc.SQLException("err2");
        }
        catch( com.sun.star.uno.Exception e ) {
            log.println("Service not available" );
            throw new StatusException("Service not available", e) ;
        }

        if (oInterface == null) {
            log.println("Service wasn't created") ;
            throw new StatusException(Status.failed("Service wasn't created")) ;
        }

        oObj = (XInterface) oInterface;

        log.println( "    creating a new environment for object" );
        TestEnvironment tEnv = new TestEnvironment( oObj );

        log.println("add ObjectRelations err1 and err2 for 'ErrorMessageDialog'");
        tEnv.addObjRelation("ERR1",err1);
        tEnv.addObjRelation("ERR2",err2);

        return tEnv;
    } // finish method getTestEnvironment

} // finish class OSQLMessageDialog

