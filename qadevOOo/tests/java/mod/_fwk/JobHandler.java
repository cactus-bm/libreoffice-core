/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: JobHandler.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:20:43 $
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

package mod._fwk;

import java.io.PrintWriter;

import lib.Status;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.XInterface;
import com.sun.star.util.URL;
import util.utils;
/**
* Test for object that implements the following interfaces :
* <ul>
*  <li><code>com::sun::star::frame::XDispatchProvider</code></li>
*  <li><code>com::sun::star::lang::XInitialization</code></li>
* </ul><p>
* @see com.sun.star.frame.XDispatchProvider
* @see com.sun.star.lang.XInitialization
* @see ifc.frame._XDispatchProvider
* @see ifc.lang._XInitialization
*/
public class JobHandler extends TestCase {

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    */
    public TestEnvironment createTestEnvironment( TestParameters Param,
        PrintWriter log ) throws StatusException {
        boolean serviceRegistered = false;

        try {
            Object obj = ((XMultiServiceFactory)Param.getMSF()).createInstance("test.Job");
            serviceRegistered = obj != null;
        } catch(com.sun.star.uno.Exception e) {}

        log.println("Service test.Job is "
            + (serviceRegistered ? "already" : "not yet")  + " registered.");
        if (! serviceRegistered){
            String message = "You have to register 'test.Job' before office is stared.\n";
            message += "Please run '$OFFICEPATH/program/pkgchk $DOCPTH/qadevlibs/JobExecutor.jar'";
            throw new StatusException(message, new Exception());
        }

        XInterface oObj = null;

        try {
            oObj = (XInterface)((XMultiServiceFactory)Param.getMSF()).createInstance(
                "com.sun.star.comp.framework.jobs.JobDispatch");
        } catch(com.sun.star.uno.Exception e) {
            e.printStackTrace(log);
            throw new StatusException(
                Status.failed("Couldn't create instance"));
        }

        TestEnvironment tEnv = new TestEnvironment( oObj );

        tEnv.addObjRelation("XDispatchProvider.URL", "vnd.sun.star.job:alias=TestJob");
        return tEnv;
    } // finish method getTestEnvironment

}
