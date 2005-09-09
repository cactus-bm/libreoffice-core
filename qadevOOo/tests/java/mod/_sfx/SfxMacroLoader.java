/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SfxMacroLoader.java,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 03:20:41 $
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

package mod._sfx;

import java.io.PrintWriter;

import lib.Status;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.dbg;
import util.utils;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.XInterface;

/**
 * Test for object which is represented by service
 * <code>com.sun.star.comp.sfx2.SfxMacroLoader</code>. <p>
 * Object implements the following interfaces :
 * <ul>
 *  <li> <code>com::sun::star::frame::XDispatchProvider</code></li>
 *  <li> <code>com::sun::star::lang::XInitialization</code></li>
 * </ul>
 * This object test <b> is NOT </b> designed to be run in several
 * threads concurently.
 * @see com.sun.star.frame.XDispatchProvider
 * @see com.sun.star.lang.XInitialization
 * @see ifc.frame._XDispatchProvider
 * @see ifc.lang._XInitialization
 */
public class SfxMacroLoader extends TestCase {

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Creates an instance of the service
    * <code>com.sun.star.comp.sfx2.SfxMacroLoader</code>. <p>
    *     Object relations created :
    * <ul>
    *  <li> <code>'FrameLoader.URL"'</code> for
    *      {@link ifc.frame._XFrameLoader} </li>
    * </ul>
    */
    protected TestEnvironment createTestEnvironment
            (TestParameters Param, PrintWriter log) {
        XInterface oObj = null;
        Object oInterface = null ;

        //now get the OButtonControl
        try {
            oInterface = ((XMultiServiceFactory)Param.getMSF()).createInstance
                                ("com.sun.star.comp.sfx2.SfxMacroLoader");
        } catch (com.sun.star.uno.Exception e) {
            log.println("Couldn't get service");
            e.printStackTrace(log);
            throw new StatusException("Couldn't get GridControl", e );
        }

        if (oInterface == null) {
            log.println("Service wasn't created") ;
            throw new StatusException(Status.failed("Service wasn't created")) ;
        }

        oObj = (XInterface) oInterface ;
        System.out.println("ImplName: "+utils.getImplName(oObj));
        dbg.printInterfaces(oObj);

        log.println( "creating a new environment for object" );
        TestEnvironment tEnv = new TestEnvironment( oObj );

        // adding relation for XFrameLoader
        tEnv.addObjRelation("XDispatchProvider.URL",
            "macro:tools.UCB.CreateFolder(\"" +
            utils.getOfficeTemp((XMultiServiceFactory)Param.getMSF()) + "/SfxMacroLoader\")") ;


        return tEnv;
    } // finish method getTestEnvironment

}

